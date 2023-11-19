#pragma once
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <memory>
#include <string_view>
#include <vector>
#include <algorithm>
#include "_main.hxx"

using std::unique_ptr;
using std::string_view;
using std::vector;
using std::make_unique;
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
 * @tparam CHECK check for error?
 * @param data input file data
 * @param symmetric is graph symmetric?
 * @param weighted is graph weighted?
 * @param fb on body line (u, v, w)
 */
template <class FB>
inline void readEdgelistFormatDoChecked(string_view data, bool symmetric, bool weighted, FB fb) {
  auto fu = [](char c) { return c==','; };                      // Support CSV
  auto fw = [](char c) { return c==',' || c=='%' || c=='#'; };  // Support CSV, comments
  auto ib = data.begin(), ie = data.end(), it = ib;
  for (; it!=ie; it = findNextLine(it, ie)) {
    // Skip past empty lines and comments.
    it = findNextNonBlank(it, ie, fu);
    if (it==ie || *it=='%' || *it=='#' || isNewline(*it)) continue;
    // Read u, v, w (if weighted).
    int64_t u = 0, v = 0; double w = 1; auto il = it;
    it = readNumberSimdW<true>(u, it, ie, fu, fw);  // Source vertex
    it = readNumberSimdW<true>(v, it, ie, fu, fw);  // Target vertex
    if (weighted) {
      it = readNumberSimdW<true>(w, it, ie, fu, fw);  // Edge weight
    }
    if (u<0 || v<0) throw FormatError("Invalid Edgelist body (negative vertex-id)", il);
    fb(u, v, w);
    if (symmetric && u!=v) fb(v, u, w);
  }
}


/**
 * Read an EdgeList format file (crazy frog version).
 * @param data input file data (updated)
 * @param symmetric is graph symmetric
 * @param weighted is graph weighted
 * @param fb on body line (u, v, w)
 */
template <class FB>
inline void readEdgelistFormatDoUnchecked(string_view data, bool symmetric, bool weighted, FB fb) {
  auto ib = data.begin(), ie = data.end(), it = ib;
  asm("vzeroupper");  // Clear AVX registers (for performance)
  while (true) {
    // Read u, v, w (if weighted).
    uint64_t u = 0, v = 0; double w = 1;
    it = findNextDigit(it, ie);
    if (it==ie) break;  // No more lines
    it = parseWholeNumberSimdW(u, it, ie);  // Source vertex
    it = findNextDigit(it, ie);
    it = parseWholeNumberSimdW(v, it, ie);  // Target vertex
    if (weighted) {
      it = findNextDigit(it, ie);
      it = parseFloatSimdW(w, it, ie);  // Edge weight
    }
    fb(u, v, w);
    if (symmetric && u!=v) fb(v, u, w);
  }
  asm("vzeroupper");  // Clear AVX registers (for performance)
}


/**
 * Read an EdgeList format file.
 * @tparam CHECK check for error?
 * @param data input file data (updated)
 * @param symmetric is graph symmetric
 * @param weighted is graph weighted
 * @param fb on body line (u, v, w)
 */
template <bool CHECK=false, class FB>
inline void readEdgelistFormatDo(string_view data, bool symmetric, bool weighted, FB fb) {
  if constexpr (CHECK) readEdgelistFormatDoChecked(data, symmetric, weighted, fb);
  else readEdgelistFormatDoUnchecked(data, symmetric, weighted, fb);
}


/**
 * Read a file in Edgelist format, and record the edges.
 * @tparam CHECK check for error?
 * @param sources source vertices (output)
 * @param targets target vertices (output)
 * @param weights edge weights (output)
 * @param data input file data
 * @param symmetric is graph symmetric?
 * @param weighted is graph weighted?
 */
template <bool CHECK=false, class IK, class IE>
inline void readEdgelistFormatW(IK sources, IK targets, IE weights, string_view data, bool symmetric, bool weighted) {
  size_t i = 0;
  readEdgelistFormatDo<CHECK>(data, symmetric, weighted, [&](auto u, auto v, auto w) {
    sources[i] = u;
    targets[i] = v;
    if (weighted) weights[i] = w;
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
 * @tparam CHECK check for error?
 * @param sources per-thread source vertices (output)
 * @param targets per-thread target vertices (output)
 * @param weights per-thread edge weights (output)
 * @param data input file data
 * @param symmetric is graph symmetric
 * @param weighted is graph weighted
 * @returns per-thread number of edges read
 */
template <bool CHECK=false, class IIK, class IIE>
inline auto readEdgelistFormatOmpW(IIK sources, IIK targets, IIE weights, string_view data, bool symmetric, bool weighted) {
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
      if (weighted) weights[t][i] = w;
      ++i;
    };
    if constexpr (CHECK) {
      try { readEdgelistFormatDo<true>(bdata, symmetric, weighted, fb); }
      catch (const FormatError& e) { if (err.empty()) err = e; }
    }
    else readEdgelistFormatDo(bdata, symmetric, weighted, fb);
    // Update per-thread index.
    *is[t] = i;
  }
  // Throw error if any.
  if (CHECK && !err.empty()) throw err;
}
#endif
#pragma endregion
#pragma endregion
