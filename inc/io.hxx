#pragma once
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <memory>
#include <string>
#include <vector>
#include <istream>
#include <sstream>

using std::unique_ptr;
using std::string;
using std::vector;
using std::istream;
using std::istringstream;
using std::make_unique;
using std::strcmp;
using std::strtoull;
using std::strtod;
using std::sscanf;
using std::fgets;




#pragma region METHODS
#pragma region READ COO FORMAT HEADER
/**
 * Read header of a COO format file, using fscanf variants.
 * @param rows number of rows (output)
 * @param cols number of columns (output)
 * @param size number of lines/edges (output)
 * @param stream input file stream
 */
inline void readCooFormatHeaderFscanfW(size_t& rows, size_t& cols, size_t& size, FILE *stream) {
  char line[1024];
  while (true) {
    fgets(line, 1024, stream);
    if (line[0]!='%' && line[0]!='#') break;
  }
  // Read rows, cols, size.
  int args = sscanf(line, "%zu %zu %zu", &rows, &cols, &size);
  if (args!=3) throw FormatError("Invalid COO header (bad sizes)");
}
#pragma endregion




#pragma region READ MTX FORMAT HEADER
/**
 * Read header of a MTX format file, using fscanf variants.
 * @param symmetric is graph symmetric (output)
 * @param rows number of rows (output)
 * @param cols number of columns (output)
 * @param size number of lines/edges (output)
 * @param stream input file stream
 */
inline void readMtxFormatHeaderFscanfW(bool& symmetric, size_t& rows, size_t& cols, size_t& size, FILE *stream) {
  char line[1024], h0[1024], h1[1024], h2[1024], h3[1024], h4[1024];
  while (true) {
    fgets(line, 1024, stream);
    if (line[0]!='%' && line[0]!='#') break;
    if (line[0]!='%' || line[1]!='%') continue;
    sscanf(line, "%s %s %s %s %s", h0, h1, h2, h3, h4);
  }
  if (strcmp(h0, "%%MatrixMarket")!=0 || strcmp(h1, "matrix")!=0 || strcmp(h2, "coordinate")!=0) throw FormatError("Invalid MTX header (unknown format)");
  symmetric = strcmp(h4, "symmetric")==0 || strcmp(h4, "skew-symmetric")==0;
  // Read rows, cols, size.
  int args = sscanf(line, "%zu %zu %zu", &rows, &cols, &size);
  if (args!=3) throw FormatError("Invalid MTX header (bad sizes)");
}
#pragma endregion




#pragma region READ EDGELIST FORMAT
/**
 * Read a file in Edgelist format, using fscanf variants.
 * @tparam CHECK check for error?
 * @param stream input file stream
 * @param symmetric is graph symmetric?
 * @param weighted is graph weighted?
 * @param fb on body line (u, v, w)
 */
template <bool CHECK=false, class FB>
inline void readEdgelistFormatFscanfDo(FILE *stream, bool symmetric, bool weighted, FB fb) {
  char line[1024];
  while (fgets(line, 1024, stream)) {
    char *l = line;
    size_t u = strtoull(l, &l, 10);
    size_t v = strtoull(l, &l, 10);
    double w = weighted? strtod(l, &l) : 1;
    if (CHECK && *l!='\0') throw FormatError("Invalid Edgelist line");
    fb(u, v, w);
    if (symmetric && u!=v) fb(v, u, w);
  }
}


#ifdef OPENMP
/**
 * Read a file in Edgelist format, using fscanf variants.
 * @tparam CHECK check for error?
 * @param stream input file stream
 * @param symmetric is graph symmetric?
 * @param weighted is graph weighted?
 * @param fb on body line (u, v, w)
 */
template <bool CHECK=false, class FB>
inline void readEdgelistFormatFscanfDoOmp(FILE *stream, bool symmetric, bool weighted, FB fb) {
  const int THREADS = omp_get_max_threads();
  const int LINES   = 128 * 1024;
  vector<char[1024]> lines(LINES);
  while (true) {
    // Read several lines from the stream.
    int READ = 0;
    for (int i=0; i<LINES; ++i, ++READ)
      if (!fgets(lines[i], 1024, stream)) break;
    if (READ==0) break;
    // Parse lines using multiple threads.
    #pragma omp parallel for schedule(dynamic, 1024)
    for (int i=0; i<READ; ++i) {
      char *l = lines[i];
      size_t u = strtoull(l, &l, 10);
      size_t v = strtoull(l, &l, 10);
      double w = weighted? strtod(l, &l) : 1;
      if (CHECK && *l!='\0') throw FormatError("Invalid Edgelist line");
      fb(u, v, w);
      if (symmetric && u!=v) fb(v, u, w);
    }
  }
}
#endif


/**
 * Read a file in Edgelist format, using fscanf variants.
 * @tparam CHECK check for error?
 * @param sources source vertices (output)
 * @param targets target vertices (output)
 * @param weights edge weights (output)
 * @param stream input file stream
 * @param symmetric is graph symmetric?
 * @param weighted is graph weighted?
 */
template <bool CHECK=false, class IK, class IE>
inline void readEdgelistFormatFscanfW(IK sources, IK targets, IE weights, FILE *stream, bool symmetric, bool weighted) {
  size_t i = 0;
  readEdgelistFormatFscanfDo<CHECK>(stream, symmetric, weighted, [&](auto u, auto v, auto w) {
    sources[i] = u;
    targets[i] = v;
    if (weighted) weights[i] = w;
    ++i;
  });
}


#ifdef OPENMP
/**
 * Read a file in Edgelist format, using fscanf variants.
 * @tparam CHECK check for error?
 * @param sources per-thread source vertices (output)
 * @param targets per-thread target vertices (output)
 * @param weights per-thread edge weights (output)
 * @param stream input file stream
 * @param symmetric is graph symmetric?
 * @param weighted is graph weighted?
 */
template <bool CHECK=false, class IIK, class IIE>
inline void readEdgelistFormatFscanfOmpW(IIK sources, IIK targets, IIE weights, FILE *stream, bool symmetric, bool weighted) {
  int THREADS = omp_get_max_threads();
  // Track current index for each thread.
  vector<unique_ptr<size_t>> is(THREADS);
  for (int t=0; t<THREADS; ++t)
    is[t] = make_unique<size_t>();
  // Read lines from the stream.
  readEdgelistFormatFscanfDoOmp<CHECK>(stream, symmetric, weighted, [&](auto u, auto v, auto w) {
    int t = omp_get_thread_num();
    size_t i = (*is[t])++;
    sources[t][i] = u;
    targets[t][i] = v;
    if (weighted) weights[t][i] = w;
  });
}
#endif
#pragma endregion
#pragma endregion
