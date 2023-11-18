#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include "inc/main.hxx"

using namespace std;




#pragma region CONFIGURATION
#ifndef KEY_TYPE
/** Type of vertex ids. */
#define KEY_TYPE uint32_t
#endif
#ifndef EDGE_VALUE_TYPE
/** Type of edge weights. */
#define EDGE_VALUE_TYPE float
#endif
#ifndef MAX_THREADS
/** Maximum number of threads to use. */
#define MAX_THREADS 1
#endif
#pragma endregion




#pragma region METHODS
/**
 * Main function.
 * @param argc argument count
 * @param argv argument values
 * @returns zero on success, non-zero on failure
 */
int main(int argc, char **argv) {
  using K = KEY_TYPE;
  using E = EDGE_VALUE_TYPE;
  char *file    = argv[1];
  bool weighted = argc>2? atoi(argv[2]) : false;
  omp_set_num_threads(MAX_THREADS);
  printf("OMP_NUM_THREADS=%d\n", MAX_THREADS);
  printf("Reading graph %s ...\n", file);
  // Read MTX file header.
  FILE *stream = fopen(file, "r");
  bool symmetric = false;
  size_t rows, cols, size;
  readMtxFormatHeaderW(symmetric, rows, cols, size, stream);
  // Allocate memory.
  vector<K*> sources(MAX_THREADS);
  vector<K*> targets(MAX_THREADS);
  vector<E*> weights(MAX_THREADS);
  for (int t=0; t<MAX_THREADS; t++) {
    sources[t] = new K[size];
    targets[t] = new K[size];
    weights[t] = weighted? new E[size] : nullptr;
  }
  // Read MTX file body.
  symmetric = false;  // We don't want the reverse edges
  float t = measureDuration([&]() {
    readEdgelistFormatOmpW(sources, targets, weights, stream, symmetric, weighted);
  });
  printf("{%09.1fms, order=%zu, size=%zu} readGraphOmp\n", t, rows, size);
  fclose(stream);
  printf("\n");
  return 0;
}
#pragma endregion
