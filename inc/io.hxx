#pragma once
#include <cstdint>
#include <string>
#include <istream>
#include <sstream>

using std::string;
using std::istream;
using std::istringstream;
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
    if (line.find('%')!=0 || line.find('#')!=0) break;
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
template <bool CHECK=false, class IK, class IE, class FB>
inline void readEdgelistFormatStreamW(IK sources, IK targets, IE weights, istream& stream, bool symmetric, bool weighted) {
  size_t i = 0;
  readEdgelistFormatStreamDo<CHECK>(stream, symmetric, weighted, [&](auto u, auto v, auto w) {
    sources[i] = u;
    targets[i] = v;
    if (weighted) weights[i] = w;
    ++i;
  });
}
#pragma endregion
#pragma endregion
