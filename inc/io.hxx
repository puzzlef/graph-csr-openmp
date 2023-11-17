#pragma once
#include <cstdint>
#include <cstdlib>
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
using std::strtoull;
using std::strtod;
using std::getline;




#pragma region METHODS
#pragma region READ COO FORMAT HEADER
/**
 * Read header of a COO format file, using stream.
 * @param rows number of rows (output)
 * @param cols number of columns (output)
 * @param size number of lines/edges (output)
 * @param stream input file stream
 */
inline void readCooFormatHeaderStreamW(size_t& rows, size_t& cols, size_t& size, istream& stream) {
  string line;
  while (true) {
    getline(stream, line);
    if (line.find('%')!=0 || line.find('#')!=0) break;
  }
  // Read rows, cols, size.
  istringstream lstream(line);
  lstream >> rows >> cols >> size;
  if (lstream.fail()) throw FormatError("Invalid COO header (bad sizes)");
}
#pragma endregion




#pragma region READ MTX FORMAT HEADER
/**
 * Read header of a MTX format file, using stream.
 * @param symmetric is graph symmetric (output)
 * @param rows number of rows (output)
 * @param cols number of columns (output)
 * @param size number of lines/edges (output)
 * @param stream input file stream
 */
inline void readMtxFormatHeaderStreamW(bool& symmetric, size_t& rows, size_t& cols, size_t& size, istream& stream) {
  string line, h0, h1, h2, h3, h4;
  while (true) {
    getline(stream, line);
    if (line.find('%')!=0 && line.find('#')!=0) break;
    if (line.find("%%")!=0) continue;
    istringstream lstream(line);
    lstream >> h0 >> h1 >> h2 >> h3 >> h4;
  }
  if (h0!="%%MatrixMarket" || h1!="matrix" || h2!="coordinate") throw FormatError("Invalid MTX header (unknown format)");
  symmetric = h4=="symmetric" || h4=="skew-symmetric";
  // Read rows, cols, size.
  istringstream lstream(line);
  lstream >> rows >> cols >> size;
  if (lstream.fail()) throw FormatError("Invalid MTX header (bad sizes)");
}
#pragma endregion




#pragma region READ EDGELIST FORMAT
/**
 * Read a file in Edgelist format, using stream.
 * @tparam CHECK check for error?
 * @param stream input file stream
 * @param symmetric is graph symmetric?
 * @param weighted is graph weighted?
 * @param fb on body line (u, v, w)
 */
template <bool CHECK=false, class FB>
inline void readEdgelistFormatStreamDo(istream& stream, bool symmetric, bool weighted, FB fb) {
  string line;
  while (getline(stream, line)) {
    size_t u = 0, v = 0; double w = 1;
    istringstream lstream(line);
    if (!(lstream >> u >> v)) break;
    if (weighted) lstream >> w;
    if (CHECK &&  lstream.fail()) throw FormatError("Invalid Edgelist line");
    fb(u, v, w);
    if (symmetric && u!=v) fb(v, u, w);
  }
}


#ifdef OPENMP
/**
 * Read a file in Edgelist format, using stream.
 * @tparam CHECK check for error?
 * @param stream input file stream
 * @param symmetric is graph symmetric?
 * @param weighted is graph weighted?
 * @param fb on body line (u, v, w)
 */
template <bool CHECK=false, class FB>
inline void readEdgelistFormatStreamDoOmp(istream& stream, bool symmetric, bool weighted, FB fb) {
  const int THREADS = omp_get_max_threads();
  const int LINES   = 128 * 1024;
  vector<string> lines(LINES);
  while (true) {
    // Read several lines from the stream.
    int READ = 0;
    for (int i=0; i<LINES; ++i, ++READ)
      if (!getline(stream, lines[i])) break;
    if (READ==0) break;
    // Parse lines using multiple threads.
    #pragma omp parallel for schedule(dynamic, 1024)
    for (int i=0; i<READ; ++i) {
      char *line = (char*) lines[i].c_str();
      size_t u = strtoull(line, &line, 10);
      size_t v = strtoull(line, &line, 10);
      double w = weighted? strtod(line, &line) : 1;
      if (CHECK && *line!='\0') throw FormatError("Invalid Edgelist line");
      fb(u, v, w);
      if (symmetric && u!=v) fb(v, u, w);
    }
  }
}
#endif


/**
 * Read a file in Edgelist format, using stream.
 * @tparam CHECK check for error?
 * @param sources source vertices (output)
 * @param targets target vertices (output)
 * @param weights edge weights (output)
 * @param stream input file stream
 * @param symmetric is graph symmetric?
 * @param weighted is graph weighted?
 */
template <bool CHECK=false, class IK, class IE>
inline void readEdgelistFormatStreamW(IK sources, IK targets, IE weights, istream& stream, bool symmetric, bool weighted) {
  size_t i = 0;
  readEdgelistFormatStreamDo<CHECK>(stream, symmetric, weighted, [&](auto u, auto v, auto w) {
    sources[i] = u;
    targets[i] = v;
    if (weighted) weights[i] = w;
    ++i;
  });
}


#ifdef OPENMP
/**
 * Read a file in Edgelist format, using stream.
 * @tparam CHECK check for error?
 * @param sources per-thread source vertices (output)
 * @param targets per-thread target vertices (output)
 * @param weights per-thread edge weights (output)
 * @param stream input file stream
 * @param symmetric is graph symmetric?
 * @param weighted is graph weighted?
 */
template <bool CHECK=false, class IIK, class IIE>
inline void readEdgelistFormatStreamOmpW(IIK sources, IIK targets, IIE weights, istream& stream, bool symmetric, bool weighted) {
  int THREADS = omp_get_max_threads();
  // Track current index for each thread.
  vector<unique_ptr<size_t>> is(THREADS);
  for (int t=0; t<THREADS; ++t)
    is[t] = make_unique<size_t>();
  // Read lines from the stream.
  readEdgelistFormatStreamDoOmp<CHECK>(stream, symmetric, weighted, [&](auto u, auto v, auto w) {
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
