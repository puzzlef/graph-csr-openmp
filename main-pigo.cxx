#include <iostream>
#include "ext/pigo.hpp"

using namespace std;
using namespace pigo;




/**
 * Main function.
 * @param argc argument count
 * @param argv argument values
 * @returns zero on success, non-zero on failure
 */
int main(int argc, char** argv) {
  Graph g { argv[1] };
  cout << "number of vertices: " << g.n() << endl;
  cout << "number of edges: " << g.m() << endl;
  cout << "vertex 0's neighbors:\n";
  for (auto n : g.neighbors(0))
    cout << n << endl;
  return 0;
}
