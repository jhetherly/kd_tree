#include <functional>
#include <iterator>
#include <initializer_list>
#include <forward_list>
#include <algorithm>

namespace Analysis {
template<class, class, class,
         class, class> class kd_tree;

namespace kd_tree_internal {
class kd_node_base {
  template<class, class, class,
           class, class> friend class Analysis::kd_tree;

  bool m_isLeaf;

public:

  kd_node_base (const bool &isLeaf) : m_isLeaf(isLeaf) {}
  kd_node_base (const kd_node_base &other) : m_isLeaf(other.m_isLeaf) {}
  kd_node_base (kd_node_base &&other) : m_isLeaf(std::move(other.m_isLeaf)) {}

  virtual ~kd_node_base () {}

  bool isLeaf () const noexcept {
    return m_isLeaf;
  }
};


template<class Key, class T,
         class Alloc = std::allocator<std::pair<const Key, const T> > >
class kd_leaf : public kd_node_base {
  template<class, class, class,
           class, class> friend class Analysis::kd_tree;

  using value_type = std::pair<const Key, const T>;

  value_type m_value;

public:

  kd_leaf (const value_type &val) : kd_node_base(true), m_value {val} {}
  kd_leaf (value_type &&val) : kd_node_base(true), m_value {std::move(val)} {}
  kd_leaf (const kd_leaf &other) : kd_node_base(true), m_value(other.m_value) {}
  kd_leaf (kd_leaf &&other) : kd_node_base(true), m_value(std::move(other.m_value)) {}

  virtual ~kd_leaf () {}

  const Key& GetKey () const noexcept {
    return m_value.first;
  }

  const T& GetValue () const noexcept {
    return m_value.second;
  }

  const value_type& GetPair () const noexcept {
    return m_value;
  }
};


template<class Key, class T,
         class Alloc = std::allocator<std::pair<const Key, const T> > >
class kd_node : public kd_node_base {
  template<class, class, class,
           class, class> friend class Analysis::kd_tree;

  using subkey_type = typename Key::value_type;
  using subkey_alloc = typename Key::allocator_type;

  subkey_alloc   m_coordAlloc;
  subkey_type   *m_median { nullptr };
  kd_node_base  *m_leftChild { nullptr },
                *m_rightChild { nullptr };

public:

  kd_node () : kd_node_base(false) {}
  kd_node (const kd_node &other) : kd_node_base(false)
  {
    if (other.m_median != nullptr) {
      this->m_median = m_coordAlloc.allocate(1);
      m_coordAlloc.construct(this->m_median, *other.m_median);
    }
    if (other.m_leftChild != nullptr) {
      if (other.m_leftChild->isLeaf())
        this->m_leftChild = new kd_leaf<Key, T, Alloc>(*static_cast<kd_leaf<Key, T, Alloc>*>(other.m_leftChild));
      else
        this->m_leftChild = new kd_node<Key, T, Alloc>(*static_cast<kd_node<Key, T, Alloc>*>(other.m_leftChild));
    }
    if (other.m_rightChild != nullptr) {
      if (other.m_rightChild->isLeaf())
        this->m_rightChild = new kd_leaf<Key, T, Alloc>(*static_cast<kd_leaf<Key, T, Alloc>*>(other.m_rightChild));
      else
        this->m_rightChild = new kd_node<Key, T, Alloc>(*static_cast<kd_node<Key, T, Alloc>*>(other.m_rightChild));
    }
  }
  kd_node (kd_node &&other) : kd_node_base(false), m_median(std::move(other.m_median)),
                              m_leftChild(std::move(other.m_leftChild)), m_rightChild(std::move(other.m_rightChild))
  {}

  virtual ~kd_node ()
  {
    if (m_median) {
      m_coordAlloc.destroy(m_median);
      m_coordAlloc.deallocate(m_median, 1);
    }
    if (m_leftChild) delete m_leftChild;
    if (m_rightChild) delete m_rightChild;
  }

  const subkey_type& GetMedian () const noexcept {
    return *m_median;
  }

  const kd_node_base* GetLeftChild () const noexcept {
    return m_leftChild;
  }

  kd_node_base* GetLeftChild () noexcept {
    return m_leftChild;
  }

  const kd_node_base* GetRightChild () const noexcept {
    return m_rightChild;
  }

  kd_node_base* GetRightChild () noexcept {
    return m_rightChild;
  }
};

} // kd_tree_internal



template<class Key, class T,
         class Compare = std::less<typename Key::value_type>,
         class Equate  = std::equal_to<typename Key::value_type>,
         class Alloc   = std::allocator<std::pair<const Key, const T> > >
class kd_tree {

  kd_tree_internal::kd_node<Key, T, Alloc> *m_root { nullptr };
  size_t  m_dim;
  Compare m_comp;
  Equate  m_equate;
  typename Key::allocator_type m_coordAlloc;
  Alloc   m_alloc;
  std::forward_list < std::reference_wrapper < const std::pair<const Key, const T>>> m_values;

  template<class RandomAccessIterator>
  typename Key::value_type*
  computeMedian (std::pair<RandomAccessIterator, RandomAccessIterator>&,
                 const unsigned long long&);
  template<class RandomAccessIterator>
  void
  checkMedian (std::pair<RandomAccessIterator, RandomAccessIterator>&,
               const unsigned long long&,
               typename Key::value_type*&);
  template<class ForwardIterator>
  std::pair < std::pair<ForwardIterator, ForwardIterator>,
  std::pair < ForwardIterator, ForwardIterator >>
  splitRange(std::pair<ForwardIterator, ForwardIterator>&,
             const unsigned long long&,
             const typename Key::value_type&);
  template<class ForwardIterator>
  std::pair < std::pair<ForwardIterator, ForwardIterator>,
  std::pair < ForwardIterator, ForwardIterator >>
  specialSplitRange(std::pair<ForwardIterator, ForwardIterator>&,
                    const Key&);
  template<class Container>
  bool CheckKey (const Container&,
                 const Key      &) const;

  struct DefaultResolution {
    template <class RandomAccessIterator>
    T operator() (RandomAccessIterator, RandomAccessIterator l) const {return std::prev(l)->second;}
  } m_defaultCR;

public:

  using key_type    = Key;
  using stored_type = T;
  using subkey_type = typename key_type::value_type;
  // using keysize_type = typename key_type::size_type;
  using value_type  = std::pair<const key_type, const stored_type>;
  using key_compare = Compare;
  // using value_compare = ;
  using allocator_type  = Alloc;
  using reference       = value_type &;
  using const_reference = const value_type &;
  using pointer         = typename std::allocator_traits<allocator_type>::pointer;
  using const_pointer   = typename std::allocator_traits<allocator_type>::const_pointer;
  using iterator        = typename decltype(m_values)::iterator;
  using const_iterator  = typename decltype(m_values)::const_iterator;
  // using reverse_iterator = ;
  // using const_reverse_iterator = ;
  using difference_type = typename std::iterator_traits<iterator>::difference_type;
  using size_type       = std::size_t;

  /**
   * Takes two random-access iterators to the begining and end of the data and
   * the dimension of the keys
   * Data should be random-access iterable (e.g. vector) and contain std::pairs
   * of the form std::pair<Key, T>
   * Key should be forward iterable
   * T can be any copiable or copy-movable type
   */
  template<class RandomAccessIterator>
  kd_tree (RandomAccessIterator, RandomAccessIterator, const size_t&);
  /**
   * CollisionResolver should return a new "stored_type" object given iterators
   * to a sequence of elements that have the same key
   */
  template<class RandomAccessIterator, class CollisionResolver>
  kd_tree (RandomAccessIterator, RandomAccessIterator, const size_t&, const CollisionResolver&);

  kd_tree (const kd_tree &other) :
    m_root(new kd_tree_internal::kd_node<Key, T, Alloc>(*other.m_root)),
    m_dim(other.m_dim), m_comp(other.m_comp), m_equate(other.m_equate),
    m_alloc(other.m_alloc), m_values(other.m_values)
  {}
  kd_tree (kd_tree &&other) :
    m_root(std::move(other.m_root)), m_dim(std::move(other.m_dim)),
    m_comp(std::move(other.m_comp)), m_equate(std::move(other.m_equate)),
    m_alloc(std::move(other.m_alloc)), m_values(std::move(other.m_values))
  {
    other.m_root = nullptr;
  }

  virtual ~kd_tree () {
    if (m_root) delete m_root;
  }

  allocator_type get_allocator () const noexcept {
    return m_alloc;
  }

  /**
   * Container should be iterable and contain min/max pairs for each coordinate
   * Returns vector of key-value pairs in "sorted" order
   */
  template<class Container>
  std::forward_list < std::reference_wrapper < const value_type >> operator[](const Container&) const;

  std::forward_list < std::reference_wrapper < const value_type >>
  operator[](std::initializer_list<std::pair<subkey_type, subkey_type>> l) const
  {return (*this)[std::forward_list<std::pair<subkey_type, subkey_type>>(l)];}

  // iterate over all elements in tree in reverse order
  iterator begin () {return m_values.begin();}
  iterator end () {return m_values.end();}
  const_iterator cbegin () const {return m_values.cbegin();}
  const_iterator cend () const {return m_values.cend();}
};
}

#include "kd_tree.icc"
