#include <algorithm>
#include <tuple>
#include <vector>
#include <cstdint>
#include <cstdlib>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <x86intrin.h>
#include <omp.h>
#include "inc/main.hxx"

using namespace std;




#pragma region CONFIGURATION
#ifndef MAX_THREADS
/** Maximum number of threads to use. */
#define MAX_THREADS 64
#endif
#pragma endregion




#pragma region METHODS
#pragma region MEMORY MAPPED FILE
/**
 * Map file to memory.
 * @param pth file path
 * @returns file descriptor, mapped data, and file size
 */
inline tuple<int, void*, size_t> mapFileToMemory(const char *pth) {
  // Open file as read-only.
  int fd = open(pth, O_RDONLY);
  if (fd==-1) return {-1, nullptr, 0};
  // Get file size.
  struct stat sb;
  if (fstat(fd, &sb)==-1) return {-1, nullptr, 0};
  // Map file to memory.
  void *addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE | MAP_NORESERVE, fd, 0);  // MAP_SHARED?
  if (addr==MAP_FAILED) return {-1, nullptr, 0};
  madvise(addr, sb.st_size, MADV_WILLNEED);  // MADV_SEQUENTIAL?
  // Return file descriptor, mapped data, and file size.
  return {fd, addr, sb.st_size};
}


/**
 * Unmap file from memory.
 * @param fd file descriptor
 * @param addr mapped data
 * @param size file size
 */
inline void unmapFileFromMemory(int fd, void *addr, size_t size) {
  munmap(addr, size);
  close(fd);
}
#pragma endregion




#pragma region ALLOCATE MEMORY
/**
 * Allocate memory using mmap.
 * @param size memory size
 * @returns allocated memory
 */
inline void* allocateMemoryMmap(size_t size) {
  return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}


/**
 * Free memory allocated using mmap.
 * @param addr memory address
 * @param size memory size
 */
inline void freeMemoryMmap(void *addr, size_t size) {
  munmap(addr, size);
}
#pragma endregion




#pragma region PERFORM EXPERIMENT
// Check if character is a digit.
inline bool isDigit(char c) {
  return c>='0' && c<='9';
}

// Skip digit characters.
template <class I>
inline I skipDigits(I ib, I ie) {
  for (; ib!=ie && isDigit(*ib); ++ib);
  return ib;
}

// Skip non-digit characters.
template <class I>
inline I skipNonDigits(I ib, I ie) {
  for (; ib!=ie && !isDigit(*ib); ++ib);
  return ib;
}

// Parse whole numbers from string.
template <class T, class I>
inline I parseWholeNumberW(T &a, I ib, I ie) {
  a = 0;
  for (; ib!=ie && isDigit(*ib); ++ib)
    a = a*10 + (*ib - '0');
  return ib;
}

// Parse whole numbers from string, using SIMD instructions.
template <class T, class I>
inline I parseWholeNumberSimdW(T &a, I ib, I ie) {
  ie = skipDigits(ib, ie);
  size_t n = size_t(ie - ib);
  const __m256i C0 = _mm256_set1_epi8('0');
  const __m256i D9 = _mm256_set1_epi8(9);
  const __m256i P1 = _mm256_set_epi8(1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10,
    1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10);
  const __m128i P2 = _mm_set_epi8(1, 100, 1, 100, 1, 100, 1, 100, 1, 100, 1, 100, 1, 100, 1, 100);
  const __m128i P4 = _mm_set_epi16(1, 10000, 1, 10000, 1, 10000, 1, 10000);
  uint32_t mask = uint32_t(0xFFFFFFFF) << (ib - ie + 32);
  auto xc = _mm256_maskz_loadu_epi8(mask, ie - 32);
  auto xd = _mm256_maskz_sub_epi8(mask, xc, C0);
  auto x2_16 = _mm256_maddubs_epi16(xd, P1);
  auto x2_08 = _mm256_cvtepi16_epi8(x2_16);
  auto x4_16 = _mm_maddubs_epi16(x2_08, P2);
  auto x8_32 = _mm_madd_epi16(x4_16, P4);
  uint64_t u = _mm_extract_epi32(x8_32, 3);
  uint64_t v = _mm_extract_epi32(x8_32, 2);
  uint64_t w = _mm_extract_epi32(x8_32, 1);
  if constexpr (sizeof(T)<=4) a = u + v*100000000;
  else a = u + v*100000000 + w*10000000000000000;
  return ie;
}

// Read integers from string.
inline size_t readIntegers(uint32_t *edges, const uint8_t *data, size_t N) {
  size_t i = 0;
  const uint8_t *ib = data;
  const uint8_t *ie = data + N;
  while (ib<ie) {
    ib = skipNonDigits(ib, ie);
    if (ib<ie) ib = parseWholeNumberW(edges[i++], ib, ie);
  }
  return i;
}

// Read integers from string, using OpenMP.
template <size_t BLOCK=256*1024>
inline size_t readIntegersOmp(uint32_t **edges, const uint8_t *data, size_t N) {
  size_t i = 0;
  const uint8_t *ib = data;
  const uint8_t *ie = data + N;
  #pragma omp parallel for schedule(dynamic, 1) reduction(+:i)
  for (size_t b=0; b<N; b+=BLOCK) {
    int t = omp_get_thread_num();
    const uint8_t *bb = ib + b;
    const uint8_t *be = min(bb + BLOCK, ie);
    if (bb!=ib && isDigit(*(bb-1))) bb = skipDigits(bb, ie);
    if (be!=ie && isDigit(*(be-1))) be = skipDigits(be, ie);
    while (bb<be) {
      bb = skipNonDigits(bb, be);
      // if (bb<be) { bb = skipDigits(bb, be); i++; }
      if (bb<be) bb = parseWholeNumberSimdW(edges[t][i++], bb, be);
    }
  }
  return i;
}




/**
 * Main function.
 * @param argc argument count
 * @param argv argument values
 * @returns zero on success, non-zero on failure
 */
int main(int argc, char **argv) {
  char  *file  = argv[1];
  bool   PAR   = argc>2 ? atoi(argv[2]) : 0;     // 0=serial, 1=parallel
  omp_set_num_threads(MAX_THREADS);
  printf("OMP_NUM_THREADS=%d\n", MAX_THREADS);
  // Map file to memory.
  auto [fd, addr, size] = mapFileToMemory(file);
  // Allocate memory for storing converted integers (overallocate 64K pages).
  int  T = PAR? MAX_THREADS : 1;
  vector<uint32_t*> integers(T);
  for (size_t t=0; t<T; ++t)
    integers[t] = (uint32_t*) allocateMemoryMmap(sizeof(uint32_t) * size / 4);
  printf("Counting and converting integers in file %s ...\n", file);
  size_t n = 0;
  float tr = measureDuration([&]() {
    if (PAR) n = readIntegersOmp(integers.data(), (uint8_t*) addr, size);
    else     n = readIntegers   (integers[0],     (uint8_t*) addr, size);
  });
  printf("{%09.1fms, n=%zu} %s\n", tr, n, PAR? "byteSumOmp" : "byteSum");
  // Free memory.
  for (size_t t=0; t<T; ++t)
    freeMemoryMmap(integers[t], sizeof(uint32_t) * size / 4);
  unmapFileFromMemory(fd, addr, size);
  printf("\n");
  return 0;
}
#pragma endregion
#pragma endregion
