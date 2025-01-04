#pragma once
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>

using std::unique_ptr;
using std::string;
using std::string_view;
using std::vector;
using std::make_unique;
using std::memcpy;
using std::strcmp;
using std::strtoull;
using std::strtod;
using std::sscanf;
using std::min;




#pragma region METHODS
#pragma region READ COO FORMAT HEADER
/**
 * Read header of a COO format file.
 * @param rows number of rows (output)
 * @param cols number of columns (output)
 * @param size number of lines/edges (output)
 * @param data input file data
 * @returns size of header
 */
inline size_t readCooFormatHeaderW(size_t& rows, size_t& cols, size_t& size, string_view data) {
  auto fu = [](char c) { return false; };
  auto fw = [](char c) { return false; };
  auto ib = data.begin(), ie = data.end(), it = ib;
  // Skip past empty lines and comments.
  for (; it!=ie; it = findNextLine(it, ie)) {
    it = findNextNonBlank(it, ie, fu);
    if (*it!='%' || *it!='#' || !isNewline(*it)) break;
  }
  // Read rows, cols, size.
  it = readNumberW<true>(rows, it, ie, fu, fw);  // Number of vertices
  it = readNumberW<true>(cols, it, ie, fu, fw);  // Number of vertices
  it = readNumberW<true>(size, it, ie, fu, fw);  // Number of edges
  // Jump to the next line.
  it = findNextLine(it, ie);
  return it-ib;
}
#pragma endregion




#pragma region READ MTX FORMAT HEADER
/**
 * Read header of a MTX format file (check for errors, handle comments).
 * @param symmetric is graph symmetric (output)
 * @param rows number of rows (output)
 * @param cols number of columns (output)
 * @param size number of lines/edges (output)
 * @param data input file data
 * @returns size of header
 */
inline size_t readMtxFormatHeaderW(bool& symmetric, size_t& rows, size_t& cols, size_t& size, string_view data) {
  auto fu = [](char c) { return false; };
  auto fw = [](char c) { return false; };
  auto ib = data.begin(), ie = data.end(), it = ib;
  // Skip past the comments and read the graph type.
  string_view h0, h1, h2, h3, h4;
  for (; it!=ie; it = findNextLine(it, ie)) {
    if (*it!='%') break;
    if (data.substr(it-ib, 14)!="%%MatrixMarket") continue;
    it = readTokenW(h0, it, ie, fu, fw);  // %%MatrixMarket
    it = readTokenW(h1, it, ie, fu, fw);  // Graph
    it = readTokenW(h2, it, ie, fu, fw);  // Format
    it = readTokenW(h3, it, ie, fu, fw);  // Field
    it = readTokenW(h4, it, ie, fu, fw);  // Symmetry
  }
  // Check the graph type.
  if (h1!="matrix" || h2!="coordinate") throw FormatError("Invalid MTX header (unknown format)", ib);
  symmetric = h4=="symmetric" || h4=="skew-symmetric";
  // Read rows, cols, size.
  it = readNumberW<true>(rows, it, ie, fu, fw);  // Number of vertices
  it = readNumberW<true>(cols, it, ie, fu, fw);  // Number of vertices
  it = readNumberW<true>(size, it, ie, fu, fw);  // Number of edges
  // Jump to the next line.
  it = findNextLine(it, ie);
  return it-ib;
}
#pragma endregion




#pragma region READ EDGELIST FORMAT
/**
 * Read a file in Edgelist format, using mmap and sscanf.
 * @tparam WEIGHTED is graph weighted?
 * @tparam BASE base vertex id (0 or 1)
 * @param data input file data
 * @param symmetric is graph symmetric?
 * @param fb on body line (u, v, w)
 */
template <bool WEIGHTED=false, int BASE=1, class FB>
inline void readEdgelistFormatDoChecked(string_view data, bool symmetric, FB fb) {
  auto fu = [](char c) { return c==','; };                      // Support CSV
  auto fw = [](char c) { return c==',' || c=='%' || c=='#'; };  // Support CSV, comments
  auto ib = data.begin(), ie = data.end(), it = ib;
  for (; it!=ie; it = findNextLine(it, ie)) {
    // Skip past empty lines and comments.
    it = findNextNonBlank(it, ie, fu);
    if (it==ie || *it=='%' || *it=='#' || isNewline(*it)) continue;
    // Read u, v, w (if weighted).
    int64_t u = 0, v = 0; double w = 1; auto il = it;
    it = readNumberW<true>(u, it, ie, fu, fw);  // Source vertex
    it = readNumberW<true>(v, it, ie, fu, fw);  // Target vertex
    if constexpr (WEIGHTED) {
      it = readNumberW<true>(w, it, ie, fu, fw);  // Edge weight
    }
    if constexpr (BASE) { --u; --v; }  // Convert to zero-based
    if (u<0 || v<0) throw FormatError("Invalid Edgelist body (negative vertex-id)", il);
    fb(u, v, w);
    if (symmetric && u!=v) fb(v, u, w);
  }
}


/**
 * Read an EdgeList format file (crazy frog version).
 * @tparam WEIGHTED is graph weighted?
 * @tparam BASE base vertex id (0 or 1)
 * @param data input file data (updated)
 * @param symmetric is graph symmetric?
 * @param fb on body line (u, v, w)
 */
template <bool WEIGHTED=false, int BASE=1, class FB>
inline void readEdgelistFormatDoUnchecked(string_view data, bool symmetric, FB fb) {
  auto ib = data.begin(), ie = data.end(), it = ib;
  while (true) {
    // Read u, v, w (if weighted).
    uint64_t u = 0, v = 0; double w = 1;
    it = findNextDigit(it, ie);
    if (it==ie) break;  // No more lines
    it = parseWholeNumberW(u, it, ie);  // Source vertex
    it = findNextDigit(it, ie);
    it = parseWholeNumberW(v, it, ie);  // Target vertex
    if constexpr (WEIGHTED) {
      it = findNextDigit(it, ie);
      it = parseFloatW(w, it, ie);  // Edge weight
    }
    if constexpr (BASE) { --u; --v; }  // Convert to zero-based
    fb(u, v, w);
    if (symmetric && u!=v) fb(v, u, w);
  }
}


/**
 * Read an EdgeList format file.
 * @tparam WEIGHTED is graph weighted?
 * @tparam BASE base vertex id (0 or 1)
 * @tparam CHECK check for error?
 * @param data input file data (updated)
 * @param symmetric is graph symmetric?
 * @param fb on body line (u, v, w)
 */
template <bool WEIGHTED=false, int BASE=1, bool CHECK=false, class FB>
inline void readEdgelistFormatDo(string_view data, bool symmetric, FB fb) {
  if constexpr (CHECK) readEdgelistFormatDoChecked<WEIGHTED, BASE>(data, symmetric, fb);
  else readEdgelistFormatDoUnchecked<WEIGHTED, BASE>(data, symmetric, fb);
}


/**
 * Read a file in Edgelist format, and record the edges.
 * @tparam WEIGHTED is graph weighted?
 * @tparam BASE base vertex id (0 or 1)
 * @tparam CHECK check for error?
 * @param degrees vertex degrees (updated)
 * @param sources source vertices (output)
 * @param targets target vertices (output)
 * @param weights edge weights (output)
 * @param data input file data
 * @param symmetric is graph symmetric?
 */
template <bool WEIGHTED=false, int BASE=1, bool CHECK=false, class IK, class IE>
inline void readEdgelistFormatToListsU(IK degrees, IK sources, IK targets, IE weights, string_view data, bool symmetric) {
  size_t i = 0;
  readEdgelistFormatDo<WEIGHTED, BASE, CHECK>(data, symmetric, [&](auto u, auto v, auto w) {
    sources[i] = u;
    targets[i] = v;
    if constexpr (WEIGHTED) weights[i] = w;
    ++degrees[u];
    ++i;
  });
}


#ifdef OPENMP
/**
 * Get characters to process for an EdgeList format block, skip first partial line [helper function].
 * @param data input file data
 * @param b block index
 * @param B block size
 * @returns characters to process for a block
 */
inline string_view readEdgelistFormatBlock(string_view data, size_t b, size_t B) {
  auto db = data.begin(), de = data.end();
  auto bb = db+b, be = min(bb+B, de);
  if (bb!=db && !isNewline(*bb-1)) bb = findNextLine(bb, de);
  if (be!=db && !isNewline(*be-1)) be = findNextLine(be, de);
  return data.substr(bb-db, be-bb);
}


/**
 * Read an EdgeList format file, and record the edges and vertex degrees.
 * @tparam WEIGHTED is graph weighted?
 * @tparam BASE base vertex id (0 or 1)
 * @tparam CHECK check for error?
 * @tparam PARTITIONS number of partitions for vertex degrees
 * @param degrees per-partition vertex degrees (updated)
 * @param sources per-thread source vertices (output)
 * @param targets per-thread target vertices (output)
 * @param weights per-thread edge weights (output)
 * @param data input file data
 * @param symmetric is graph symmetric
 * @returns per-thread number of edges read
 */
template <bool WEIGHTED=false, int BASE=1, bool CHECK=false, int PARTITIONS=4, class IIK, class IIE>
inline vector<unique_ptr<size_t>> readEdgelistFormatToListsOmpU(IIK degrees, IIK sources, IIK targets, IIE weights, string_view data, bool symmetric) {
  const size_t DATA  = data.size();
  const size_t BLOCK = 256 * 1024;  // Characters per block (256KB)
  const int T = omp_get_max_threads();
  FormatError err;  // Common error
  // Allocate space for per-thread index.
  vector<unique_ptr<size_t>> is(T);
  for (int t=0; t<T; ++t)
    is[t] = make_unique<size_t>();
  // Process a grid in parallel with dynamic scheduling.
  #pragma omp parallel for schedule(dynamic, 1) shared(err)
  for (size_t b=0; b<DATA; b+=BLOCK) {
    int    t = omp_get_thread_num();
    // Get per-thread index.
    size_t i = *is[t];
    // Skip if error occurred.
    if (CHECK && !err.empty()) continue;
    // Read a block of data.
    string_view bdata = readEdgelistFormatBlock(data, b, BLOCK);
    auto fb = [&](auto u, auto v, auto w) {
      // Record the edge.
      sources[t][i] = u;
      targets[t][i] = v;
      if constexpr (WEIGHTED) weights[t][i] = w;
      #pragma omp atomic
      ++degrees[t % PARTITIONS][u];
      ++i;
    };
    if constexpr (CHECK) {
      try { readEdgelistFormatDo<WEIGHTED, BASE, true>(bdata, symmetric, fb); }
      catch (const FormatError& e) { if (err.empty()) err = e; }
    }
    else readEdgelistFormatDo<WEIGHTED, BASE>(bdata, symmetric, fb);
    // Update per-thread index.
    *is[t] = i;
  }
  // Throw error if any.
  if (CHECK && !err.empty()) throw err;
  return is;
}
#endif
#pragma endregion




#pragma region CONVERT TO CSR FORMAT
/**
 * Convert Edgelist format to CSR format.
 * @tparam WEIGHTED is graph weighted?
 * @param offsets CSR offsets (output)
 * @param edgeKeys CSR edge keys (output)
 * @param edgeValues CSR edge values (output)
 * @param degrees vertex degrees
 * @param sources source vertices
 * @param targets target vertices
 * @param weights edge weights
 * @param rows number of rows/vertices
 */
template <bool WEIGHTED=false, class IO, class IK, class IE>
inline void convertEdgelistToCsrListsW(IO offsets, IK edgeKeys, IE edgeValues, IK degrees, IK sources, IK targets, IE weights, size_t rows) {
  // Compute offsets.
  exclusiveScanW(&offsets[0], &degrees[0][0], rows+1);
  // Populate CSR format.
  for (size_t i=0; i<rows; ++i) {
    size_t u = sources[i];
    size_t v = targets[i];
    size_t j = offsets[u]++;
    edgeKeys[j] = v;
    if constexpr (WEIGHTED) edgeValues[j] = weights[i];
  }
  // Fix offsets.
  memcpy(&offsets[1], &offsets[0], rows * sizeof(offsets[0]));
  offsets[0] = 0;
}


/**
 * Convert Edgelist format to CSR format.
 * @tparam WEIGHTED is graph weighted?
 * @tparam PARTITIONS number of partitions for vertex degrees
 * @param offsets CSR offsets (output)
 * @param edgeKeys CSR edge keys (output)
 * @param edgeValues CSR edge values (output)
 * @param poffsets per-partition CSR offsets (output)
 * @param pedgeKeys per-partition CSR edge keys (output)
 * @param pedgeValues per-partition CSR edge values (output)
 * @param degrees per-partition vertex degrees
 * @param sources per-thread source vertices
 * @param targets per-thread target vertices
 * @param weights per-thread edge weights
 * @param counts per-thread number of edges read
 * @param rows number of rows/vertices
 */
template <bool WEIGHTED=false, int PARTITIONS=4, class IO, class IK, class IE, class IIO, class IIK, class IIE>
inline void convertEdgelistToCsrListsOmpW(IO offsets, IK edgeKeys, IE edgeValues, IIO poffsets, IIK pedgeKeys, IIE pedgeValues, IIK degrees, IIK sources, IIK targets, IIE weights, const vector<unique_ptr<size_t>>& counts, size_t rows) {
  int T = omp_get_max_threads();
  vector<size_t> buf(T);
  // Compute offsets.
  if (PARTITIONS==1) exclusiveScanOmpW(&offsets[0], &buf[0], &degrees[0][0], rows+1);
  else {
    for (int t=0; t<PARTITIONS; ++t)
      exclusiveScanOmpW(&poffsets[t][0], &buf[0], &degrees[t][0], rows+1);
  }
  if (PARTITIONS==1) {
    // Populate CSR format.
    #pragma omp parallel
    {
      int t = omp_get_thread_num();
      size_t I = *counts[t];
      for (size_t i=0; i<I; ++i) {
        size_t u = sources[t][i];
        size_t v = targets[t][i];
        size_t j = 0;
        #pragma omp atomic capture
        j = offsets[u]++;
        edgeKeys[j] = v;
        if constexpr (WEIGHTED) edgeValues[j] = weights[t][i];
      }
    }
    // Fix offsets.
    memcpy(&offsets[1], &offsets[0], rows * sizeof(offsets[0]));
    offsets[0] = 0;
  }
  else {
    // Populate per-partition CSR format.
    #pragma omp parallel
    {
      int t = omp_get_thread_num();
      size_t I = *counts[t];
      for (size_t i=0; i<I; ++i) {
        size_t u = sources[t][i];
        size_t v = targets[t][i];
        size_t j = 0;
        #pragma omp atomic capture
        j = poffsets[t % PARTITIONS][u]++;
        pedgeKeys[t % PARTITIONS][j] = v;
        if constexpr (WEIGHTED) pedgeValues[t % PARTITIONS][j] = weights[t][i];
      }
    }
    // Fix per-partition offsets.
    #pragma omp parallel
    {
      int t = omp_get_thread_num();
      if (t<PARTITIONS) memcpy(&poffsets[t][1], &poffsets[t][0], rows * sizeof(poffsets[t][0]));
      if (t<PARTITIONS) poffsets[t][0] = 0;
    }
    // Combine per-partition offsets.
    #pragma omp parallel for schedule(static, 2048)
    for (size_t u=0; u<rows; ++u) {
      if (PARTITIONS==1) {}
      else if (PARTITIONS==2) degrees[0][u] += degrees[1][u];
      else if (PARTITIONS==4) degrees[0][u] += degrees[1][u] + degrees[2][u] + degrees[3][u];
      else if (PARTITIONS==8) degrees[0][u] += degrees[1][u] + degrees[2][u] + degrees[3][u] + degrees[4][u] + degrees[5][u] + degrees[6][u] + degrees[7][u];
      else {
        for (int t=1; t<PARTITIONS; ++t)
          degrees[0][u] += degrees[t][u];
      }
    }
    // Compute global offsets.
    exclusiveScanOmpW(&offsets[0], &buf[0], &degrees[0][0], rows+1);
    // Combine per-partition CSR format.
    #pragma omp parallel for schedule(dynamic, 2048)
    for (size_t u=0; u<rows; ++u) {
      size_t j = offsets[u];
      for (int t=0; t<PARTITIONS; ++t) {
        size_t i = poffsets[t][u];
        size_t I = poffsets[t][u+1];
        for (; i<I; ++i, ++j) {
          edgeKeys[j] = pedgeKeys[t][i];
          if constexpr (WEIGHTED) edgeValues[j] = pedgeValues[t][i];
        }
      }
    }
  }
}
#pragma endregion
#pragma endregion
