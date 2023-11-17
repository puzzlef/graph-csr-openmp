#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include "inc/main.hxx"

using namespace std;




#pragma region CONFIGURATION
#ifndef TYPE
/** Type of edge weights. */
#define TYPE float
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
  char *file    = argv[1];
  bool weighted = argc>2? atoi(argv[2]) : false;
  // Read MTX file header.
  ifstream stream(file);
  bool symmetric = false;
  size_t rows, cols, size;
  readMtxFormatHeaderStreamW(symmetric, rows, cols, size, stream);
  // Allocate memory.
  size_t *sources = new size_t[size];
  size_t *targets = new size_t[size];
  TYPE   *weights = weighted? new TYPE[size] : nullptr;
  // Read MTX file body.
  float t = measureDuration([&]() {
    readEdgelistFormatStreamW(sources, targets, weights, stream, symmetric, weighted);
  });
  printf("{%09.1fms} readGraph\n", t);
  printf("\n");
  return 0;
}
#pragma endregion
