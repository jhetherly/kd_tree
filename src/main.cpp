// compile with
// clang++ -std=c++11 -I. -Wall src/main.cpp -o bin/kD-tree


#include <iostream>
#include <iterator>
#include <vector>

#include "kd_tree/kd_tree.h"

// ///////////////////////////
#include <type_traits>
#include <utility>
#include <string>

class VarBase
{
public:
  enum class Type {Bool,
                   Float, Double,
                   Int, Unsigned,
                   Long, UnsignedLong,
                   Char, UnsignedChar,
                   String,
                   Unknown};

  VarBase (Type &&type) : m_type(std::move(type)) {}
  VarBase (const Type &type) : m_type(type) {}
  virtual ~VarBase () {}

  const Type& GetType () const {return m_type;}

protected:
  Type m_type;
};


struct VarBaseSortable : public VarBase
{
  VarBaseSortable (Type &&type) : VarBase(std::move(type)) {}
  VarBaseSortable (const Type &type) : VarBase(type) {}
  virtual ~VarBaseSortable () {}

  virtual bool operator< (const VarBaseSortable&) const = 0;
  virtual bool operator== (const VarBaseSortable&) const = 0;

  struct Less
  {
    bool operator() (const VarBaseSortable *lhs, const VarBaseSortable *rhs) const {return (*lhs < *rhs);}
  };

  struct Equate
  {
    bool operator() (const VarBaseSortable *lhs, const VarBaseSortable *rhs) const {return (*lhs == *rhs);}
  };
};




template <class T>
class Var : public VarBaseSortable
{
  T m_value;

public:
  Var (T &&val) :
    VarBaseSortable(std::is_same<T, bool>::value ? VarBase::Type::Bool :
                    std::is_same<T, float>::value ? VarBase::Type::Float :
                    std::is_same<T, double>::value ? VarBase::Type::Double :
                    std::is_same<T, int>::value ? VarBase::Type::Int :
                    std::is_same<T, unsigned>::value ? VarBase::Type::Unsigned :
                    std::is_same<T, long>::value ? VarBase::Type::Long :
                    std::is_same<T, unsigned long>::value ? VarBase::Type::UnsignedLong :
                    std::is_same<T, char>::value ? VarBase::Type::Char :
                    std::is_same<T, unsigned char>::value ? VarBase::Type::UnsignedChar :
                    VarBase::Type::Unknown),
    m_value(std::move(val)) {}
  Var (const T &val) :
    VarBaseSortable(std::is_same<T, bool>::value ? VarBase::Type::Bool :
                    std::is_same<T, float>::value ? VarBase::Type::Float :
                    std::is_same<T, double>::value ? VarBase::Type::Double :
                    std::is_same<T, int>::value ? VarBase::Type::Int :
                    std::is_same<T, unsigned>::value ? VarBase::Type::Unsigned :
                    std::is_same<T, long>::value ? VarBase::Type::Long :
                    std::is_same<T, unsigned long>::value ? VarBase::Type::UnsignedLong :
                    std::is_same<T, char>::value ? VarBase::Type::Char :
                    std::is_same<T, unsigned char>::value ? VarBase::Type::UnsignedChar :
                    VarBase::Type::Unknown),
    m_value(val) {}
  virtual ~Var () {}

  T& Value () {return m_value;}
  const T& Value () const {return m_value;}

  operator T () {return m_value;}
  operator const T () const {return m_value;}

  virtual bool operator< (const VarBaseSortable &rhs) const
  {
    if (rhs.GetType() == VarBase::Type::Bool) return (m_value < static_cast<const Var<bool>&>(rhs));
    if (rhs.GetType() == VarBase::Type::Float) return (m_value < static_cast<const Var<float>&>(rhs));
    if (rhs.GetType() == VarBase::Type::Double) return (m_value < static_cast<const Var<double>&>(rhs));
    if (rhs.GetType() == VarBase::Type::Int) return (m_value < static_cast<const Var<int>&>(rhs));
    if (rhs.GetType() == VarBase::Type::Unsigned) return (static_cast<const unsigned>(m_value) < static_cast<const Var<unsigned>&>(rhs));
    if (rhs.GetType() == VarBase::Type::Long) return (m_value < static_cast<const Var<long>&>(rhs));
    if (rhs.GetType() == VarBase::Type::UnsignedLong) return (static_cast<const unsigned long>(m_value) < static_cast<const Var<unsigned long>&>(rhs));
    if (rhs.GetType() == VarBase::Type::Char) return (m_value < static_cast<const Var<char>&>(rhs));
    if (rhs.GetType() == VarBase::Type::UnsignedChar) return (static_cast<const unsigned char>(m_value) < static_cast<const Var<unsigned char>&>(rhs));
    return false;
  }

  virtual bool operator== (const VarBaseSortable &rhs) const
  {
    if (rhs.GetType() == VarBase::Type::Bool) return (m_value == static_cast<const Var<bool>&>(rhs));
    if (rhs.GetType() == VarBase::Type::Float) return (m_value == static_cast<const Var<float>&>(rhs));
    if (rhs.GetType() == VarBase::Type::Double) return (m_value == static_cast<const Var<double>&>(rhs));
    if (rhs.GetType() == VarBase::Type::Int) return (m_value == static_cast<const Var<int>&>(rhs));
    if (rhs.GetType() == VarBase::Type::Unsigned) return (static_cast<const unsigned>(m_value) == static_cast<const Var<unsigned>&>(rhs));
    if (rhs.GetType() == VarBase::Type::Long) return (m_value == static_cast<const Var<long>&>(rhs));
    if (rhs.GetType() == VarBase::Type::UnsignedLong) return (static_cast<const unsigned long>(m_value) == static_cast<const Var<unsigned long>&>(rhs));
    if (rhs.GetType() == VarBase::Type::Char) return (m_value == static_cast<const Var<char>&>(rhs));
    if (rhs.GetType() == VarBase::Type::UnsignedChar) return (static_cast<const unsigned char>(m_value) == static_cast<const Var<unsigned char>&>(rhs));
    return false;
  }

};

template <>
class Var<std::string> : public VarBaseSortable
{
  std::string m_value;

public:
  Var (std::string &&val) :
    VarBaseSortable(VarBase::Type::String),
    m_value(std::move(val)) {}
  Var (std::string &val) :
    VarBaseSortable(VarBase::Type::String),
      m_value(val) {}
  virtual ~Var () {}

  std::string& Value () {return m_value;}
  const std::string& Value () const {return m_value;}

  virtual bool operator< (const VarBaseSortable &rhs) const
  {
    // using dynamic_cast instead of static_cast in case someone else needs a
    // different specialization for VarBase::Type::String (e.g. const char*)
    if (rhs.GetType() == VarBase::Type::String)
      return (m_value < dynamic_cast<const Var<std::string>&>(rhs).Value());
    return false;
  }

  virtual bool operator== (const VarBaseSortable &rhs) const
  {
    // using dynamic_cast instead of static_cast in case someone else needs a
    // different specialization for VarBase::Type::String (e.g. const char*)
    if (rhs.GetType() == VarBase::Type::String)
      return (m_value == dynamic_cast<const Var<std::string>&>(rhs).Value());
    return false;
  }
};
// ///////////////////////////


using namespace std;
int main (int argc, const char *argv[])
{
  cout << "k-D tree test\n" << endl;

  // pairs of (coordinates, "values")
  vector < pair < vector<double>, int >> points { { { 10, 1, 1 }, 1 },
                                                  { { 8, 3, 3 }, 2 },
                                                  { { 1, 3, 3 }, 3 },
                                                  { { 5, 5, 5 }, 4 },
                                                  { { 1, 4, 4 }, 5 },
                                                  { { 7, 2, 2 }, 6 },
                                                  { { 3, 4, 4 }, 7 },
                                                  { { 2, 2, 2 }, 8 } };

  cout << "Set of initial points (\"values\" are just the point index):" << endl;
  for (auto &p : points) {
    cout << p.second << ": ";
    for (auto &c : p.first) {
      cout << c << " ";
    }
    cout << "\n";
  }
  cout << endl;

  // ///////////////////////////
  {
    Analysis::kd_tree<vector<double>, int> tree(points.begin(), points.end(), 3);
    cout << "# of elements in tree: " << std::distance(tree.begin(), tree.end()) << endl;
    cout << "elements of tree in reverse sorted order: ";
    for (auto p : tree) cout << p.get().second << " ";
    cout << "\n" << endl;

    vector < pair < double, double >> constraints = { { 4, 8 }, { 1, 5 }, { 1, 3 } };

    auto contained = tree[{ { 4, 8 }, { 1, 5 }, { 1, 3 } }];
    cout << "Set of constraints:" << endl;
    for (auto &p : constraints) {
      cout << p.first << " " << p.second << "\n";
    }
    cout << endl;
    // NOTE: one must create a new container for the contained points if you are to use them as input to a kD-tree
    vector < pair < vector<double>, int >> contained_points;
    cout << "# of contained elements: " << std::distance(contained.begin(), contained.end()) << endl;
    cout << "contained elements of tree in reverse sorted order: ";
    for (auto p : contained) {
      cout << p.get().second << " ";
      contained_points.emplace_back(p.get());
    }
    cout << "\n" << endl;

    Analysis::kd_tree<vector<double>, int> tree2(contained_points.begin(), contained_points.end(), 3);
    cout << "# of elements in newly constructed tree from contained points: " <<
      std::distance(tree2.begin(), tree2.end()) << endl;
    cout << "elements of tree in reverse sorted order: ";
    for (auto p : tree2) cout << p.get().second << " ";
    cout << "\n" << endl;

    Analysis::kd_tree<vector<double>, int> tree_copy(tree);
    cout << "# of elements in copied tree: " << std::distance(tree.begin(), tree.end()) << endl;
    cout << "elements of copied tree in reverse sorted order: ";
    for (auto p : tree) cout << p.get().second << " ";
    cout << "\n\n" << endl;
  }
  // ///////////////////////////


  // ///////////////////////////
  {
    cout << "Using custom class as coordinate variables (same coordinates):" << endl;
    vector<vector<VarBaseSortable*>> data = {{new Var<double>(10), new Var<int>(1), new Var<string>("1")},
                                        {new Var<double>(8), new Var<int>(3), new Var<string>("3")},
                                        {new Var<double>(1), new Var<int>(3), new Var<string>("3")},
                                        {new Var<double>(5), new Var<int>(5), new Var<string>("5")},
                                        {new Var<double>(1), new Var<int>(4), new Var<string>("4")},
                                        {new Var<double>(7), new Var<int>(2), new Var<string>("2")},
                                        {new Var<double>(3), new Var<int>(4), new Var<string>("4")},
                                        {new Var<double>(2), new Var<int>(2), new Var<string>("2")}};
    vector < pair < vector<VarBaseSortable*>, int >> points_ptrs { { { data[0][0], data[0][1], data[0][2] }, 1 },
                                                              { { data[1][0], data[1][1], data[1][2] }, 2 },
                                                              { { data[2][0], data[2][1], data[2][2] }, 3 },
                                                              { { data[3][0], data[3][1], data[3][2] }, 4 },
                                                              { { data[4][0], data[4][1], data[4][2] }, 5 },
                                                              { { data[5][0], data[5][1], data[5][2] }, 6 },
                                                              { { data[6][0], data[6][1], data[6][2] }, 7 },
                                                              { { data[7][0], data[7][1], data[7][2] }, 8 } };
    Analysis::kd_tree<vector<VarBaseSortable*>, int,
                      VarBaseSortable::Less, VarBaseSortable::Equate> tree(points_ptrs.begin(), points_ptrs.end(), 3);

    cout << "# of elements in tree: " << std::distance(tree.begin(), tree.end()) << endl;
    cout << "elements of tree in reverse sorted order: ";
    for (auto p : tree) cout << p.get().second << " ";
    cout << "\n" << endl;

    vector < pair < VarBaseSortable*, VarBaseSortable* >> constraints = { { new Var<double>(4), new Var<double>(8) },
                                                                              { new Var<int>(1), new Var<int>(5) },
                                                                              { new Var<string>("1"), new Var<string>("3") } };
    auto contained = tree[constraints];
    cout << "# of contained elements: " << std::distance(contained.begin(), contained.end()) << endl;
    cout << "contained elements of tree in reverse sorted order: ";
    for (auto p : contained) cout << p.get().second << " ";
    cout << endl;

    for (auto &v : data)
      for (auto &e : v)
        delete e;
    for (auto &p : constraints) {
      delete p.first;
      delete p.second;
    }
  }
  // ///////////////////////////

  return 0;
} // main
