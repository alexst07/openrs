#ifndef __ERISED_MATRIX_BASE__
#define __ERISED_MATRIX_BASE__

#include <initializer_list>
#include <functional>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/utility/enable_if.hpp>

namespace erised {

enum class Axis{
  ROW,
  COL
};

template<size_t N>
class Pos {
};

template<>
class Pos<2> {
 public:
  Pos(size_t x, size_t y)
    : x_(x)
    , y_(y) {}

  size_t X() const {
    return x_;
  }

  size_t Y() const {
    return y_;
  }

 private:
  size_t x_, y_;
};

template<typename T, size_t N>
class MatBase {
 public:
  static constexpr size_t order = N;

  using value_type = T;

  MatBase() = default;
  virtual ~MatBase() = default;

  virtual T operator()(const Pos<order>& pos) const = 0;
};

template<typename T>
class Mat2d: public MatBase<T, 2> {
 public:
  using value_type = T;
  static constexpr size_t order = MatBase<T, 2>::order;

  Mat2d() = default;
  virtual ~Mat2d() = default;

  virtual T operator()(const Pos<2>& pos) const = 0;
};

template<typename T, class Derived>
class MatContinuous: Mat2d<T> {

  // Iterator class
  template <class Ref>
  class Iter;

 public:
  using value_type = T;

  static constexpr size_t order = MatBase<T, 2>::order;

  MatContinuous() = default;
  virtual ~MatContinuous() = default;

  virtual T operator()(const Pos<2>& pos) const = 0;

 private:
  template <class Value>
  class Iter
    : public boost::iterator_facade<Iter<Value>
        , T
        , boost::forward_traversal_tag> {
    struct enabler {};

   public:
    Iter(Value* p): p_(p) {}
    Iter(Value* p, size_t pos): p_(p) {}

    // make Iter's converting constructor disappear
    // when the ref_ conversion would fail
    template <class OtherValue>
    Iter(
      Iter<OtherValue> const& other
      , typename boost::enable_if<
          boost::is_convertible<OtherValue*,Value*>
        , enabler
      >::type = enabler()
    ) : p_(other.ref_) {}

   private:
    friend class boost::iterator_core_access;

    bool equal(Iter<Value> const& other) const {
      return p_ == other.p_;
    }

    void increment() { ++p_; }

    Value& dereference() const { return *p_; }

    Value* p_;
  };
};

template<class T, class Derived>
class MatDiscontinuous: Mat2d<T> {

  // Iterator class
  template <class Ref>
  class Iter;

 public:
  using value_type = T;
  typedef Iter<Derived> iterator;
  typedef Iter<Derived const> const_iterator;

  static constexpr size_t order = MatBase<T, 2>::order;

  MatDiscontinuous() = default;
  virtual ~MatDiscontinuous() = default;

  virtual T operator()(const Pos<2>& pos) const = 0;

  virtual const typename Derived::iter_type& operator[](size_t) const = 0;

  virtual typename Derived::iter_type& operator[](size_t) = 0;

 private:
    /**
   * @class Iter
   * @brief Iterator class for MatDiscontinuous
   *
   * Different from others iterators where the pointer
   * is used, on this case, the data isn't continuous
   * so, only the derivated class from MatDiscontinuous
   * knows to access the elements in the correct order,
   * the other solution is use functional program to
   * say to Iter how to iterate over data
   */
  template <class Ref>
  class Iter
    : public boost::iterator_facade<Iter<Ref>
        , T
        , boost::forward_traversal_tag> {
    struct enabler {};

   public:
    Iter(Ref& ref): ref_(ref), pos_(0) {}
    Iter(Ref& ref, size_t pos): ref_(ref), pos_(pos) {}

    // make Iter's converting constructor disappear
    // when the ref_ conversion would fail
    template <class OtherRef>
    Iter(
      Iter<OtherRef> const& other
      , typename boost::enable_if<
          boost::is_convertible<OtherRef*,Ref*>
        , enabler
      >::type = enabler()
    ) : ref_(other.ref_) {}

   private:
    friend class boost::iterator_core_access;

    bool equal(Iter<Ref> const& other) const {
      return this->pos_ == other.pos_;
    }

    void increment() { ++pos_; }

    typename Derived::iter_type& dereference() const { return ref_[pos_]; }

    Ref& ref_;
    size_t pos_;
  };

};


/**
 *
 *
 */
template<typename T>
class DataBase: Mat2d<T> {
 public:
  using value_type = T;

  DataBase() = default;
  virtual ~DataBase() = default;

  virtual T operator()(const Pos<2>& pos) const = 0;

  virtual size_t NumElements() const = 0;

  virtual size_t NumElementsLine(size_t i) const = 0;

  virtual size_t NumElementsCol(size_t i) const = 0;
};

}
#endif //__ERISED_MATRIX_BASE__
