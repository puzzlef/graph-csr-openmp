#include <iostream>
#include "ext/pigo.hpp"
#include "inc/main.hxx"

using namespace std;




#pragma region CONFIGURATION
#ifndef MAX_THREADS
/** Maximum number of threads to use. */
#define MAX_THREADS 64
#endif
#ifndef REPEAT_METHOD
/** Number of times to repeat each method. */
#define REPEAT_METHOD 5
#endif
#pragma endregion




/**
 * Main function.
 * @param argc argument count
 * @param argv argument values
 * @returns zero on success, non-zero on failure
 */
int main(int argc, char** argv) {
  char *file = argv[1];
  int repeat = REPEAT_METHOD;
  printf("Loading graph %s ...\n", file);
  for (int i=0; i<repeat; ++i) {
    auto  t0 = timeNow();
    pigo::Graph x { file };
    auto  t1 = timeNow();
    printf("order: %zu, size: %zu {}\n", size_t(x.n()), size_t(x.m()));
    float tl = duration(t0, t1);
    printf("{%09.1fms} pigo_Graph\n", tl);
    float ts = measureDuration([&]() { x.sort(); });
    printf("{%09.1fms} pigo_Graph_sort\n", ts);
    x.free();
  }
  printf("\n");
  return 0;
}
