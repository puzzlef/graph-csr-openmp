#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <memory>
#include <string_view>
#include <vector>
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
#define MAX_THREADS 64
#endif
#ifndef NUM_PARTITIONS
/** Number of partitions to use. */
#define NUM_PARTITIONS 4
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
  using O = size_t;
  using K = KEY_TYPE;
  using E = EDGE_VALUE_TYPE;
  char *file    = argv[1];
  bool weighted = argc>2? atoi(argv[2]) : false;
  omp_set_num_threads(MAX_THREADS);
  printf("OMP_NUM_THREADS=%d\n", MAX_THREADS);
  printf("NUM_PARTITIONS=%d\n", NUM_PARTITIONS);
  printf("Reading graph %s ...\n", file);
  // Read MTX file header.
  MappedFile mf(file);
  string_view data((const char*) mf.data(), mf.size());
  bool symmetric = false; size_t rows = 0, cols = 0, size = 0;
  size_t head = readMtxFormatHeaderW(symmetric, rows, cols, size, data);
  data.remove_prefix(head);
  // Allocate space for CSR.
  DiGraphCsr<K, None, E> xc;
  const size_t N = max(rows, cols);
  const size_t M = size;
  xc.resize(N, M);
  // Allocate space for sources, targets, weights, degrees, offsets, edge keys, and edge values.
  const int T = omp_get_max_threads();
  vector<size_t> counts;
  vector<K*> degrees(T);
  vector<K*> sources(T);
  vector<K*> targets(T);
  vector<E*> weights(T);
  vector<O*> poffsets(NUM_PARTITIONS);
  vector<K*> pedgeKeys(NUM_PARTITIONS);
  vector<E*> pedgeValues(NUM_PARTITIONS);
  for (int t=0; t<T; t++) {
    sources[t] = new K[size];
    targets[t] = new K[size];
    weights[t] = weighted? new E[size] : nullptr;
  }
  for (int p=0; p<NUM_PARTITIONS; p++) {
    degrees[p]     = new K[rows+1];
    poffsets[p]    = new O[rows+1];
    pedgeKeys[p]   = new K[size];
    pedgeValues[p] = weighted? new E[size] : nullptr;
  }
  // Read MTX file body.
  symmetric = false;  // We don't want the reverse edges
  float t = measureDuration([&]() {
    if (weighted) counts = readEdgelistFormatToListsOmpU<true,  1, false, NUM_PARTITIONS>(degrees.data(), sources.data(), targets.data(), weights.data(), data, symmetric);
    else          counts = readEdgelistFormatToListsOmpU<false, 1, false, NUM_PARTITIONS>(degrees.data(), sources.data(), targets.data(), weights.data(), data, symmetric);
    convertEdgelistToCsrListsOmpW<false, NUM_PARTITIONS>(xc.offsets.data(), xc.edgeKeys.data(), xc.edgeValues.data(), poffsets.data(), pedgeKeys.data(), pedgeValues.data(), degrees.data(), sources.data(), targets.data(), weights.data(), counts, rows);
  });
  // Calculate total number of edges read.
  size_t read = 0;
  for (int t=0; t<MAX_THREADS; t++)
    read += counts[t];
  printf("{%09.1fms, order=%zu, size=%zu, read=%zu} readGraphOmp\n", t, rows, size, read);
  printf("\n");
  return 0;
}
#pragma endregion
