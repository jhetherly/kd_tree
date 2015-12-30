// compile with
// clang++ -std=c++11 -I. -Wall src/main.cpp -o bin/kD-tree


#include <iostream>
#include <iterator>
#include <vector>

#include "kd_tree/kd_tree.h"

using namespace std;
int main (int argc, const char *argv[])
{
  cout << "k-D tree test" << endl;

  // pairs of (coordinates, "values")
  vector < pair < vector<double>, int >> points { { { 10, 1, 1 }, 1 },
                                                  { { 8, 3, 3 }, 2 },
                                                  { { 1, 3, 3 }, 3 },
                                                  { { 5, 5, 5 }, 4 },
                                                  { { 1, 4, 4 }, 5 },
                                                  { { 7, 2, 2 }, 6 },
                                                  { { 3, 4, 4 }, 7 },
                                                  { { 2, 2, 2 }, 8 } };

  Analysis::kd_tree<vector<double>, int> tree(points.begin(), points.end(), 3);
  cout << "# of elements in tree " << std::distance(tree.begin(), tree.end()) << endl;
  for (auto p : tree) cout << p.get().second << " ";
  cout << endl;

  // vector < pair < double, double >> constraints = { { 4, 8 }, { 1, 5 }, { 1, 3 } };

  auto contained = tree[{ { 4, 8 }, { 1, 5 }, { 1, 3 } }];
  cout << "# of contained elements " << std::distance(contained.begin(), contained.end()) << endl;

  for (auto p : contained) cout << p.get().second << " ";
  cout << endl;

  Analysis::kd_tree<vector<double>, int> tree2(contained.begin(), contained.end(), 3);
  cout << "# of elements in tree " << std::distance(tree2.begin(), tree2.end()) << endl;
  for (auto p : tree2) cout << p.get().second << " ";
  cout << endl;

  return 0;
} // main
