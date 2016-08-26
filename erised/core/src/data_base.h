#pragma once

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
  Pos(size_t x, size_t y): x_(x), y_(y) {}

  size_t X() const {
    return x_;
  }

  size_t Y() const {
    return y_;
  }

 private:
  size_t x_, y_;
};

template<>
class Pos<1> {
 public:
  Pos(size_t i): i_(i) {}

  size_t operator()() const {
    return i_;
  }

 private:
  size_t i_;
};

template<typename T, size_t N>
class MatBase {
 public:
  static constexpr size_t order = N;

  using value_type = T;

  MatBase() = default;
  virtual ~MatBase() = default;

  virtual value_type& operator()(const Pos<order>& pos) = 0;

  virtual const value_type& operator()(const Pos<order>& pos) const = 0;
};

template<typename T>
class Mat2d: public MatBase<T, 2> {
 public:
  using value_type = T;
  static constexpr size_t order = MatBase<T, 2>::order;

  Mat2d() = default;
  virtual ~Mat2d() = default;

  virtual const value_type& operator()(size_t x, size_t y) const = 0;

  virtual value_type& operator()(size_t x, size_t y) = 0;

  virtual value_type& operator()(const Pos<2>& pos) {
    return this->operator()(pos.X(), pos.Y());
  }

  virtual const value_type& operator()(const Pos<2>& pos) const {
    return this->operator()(pos.X(), pos.Y());
  }
};

template<typename T>
class Vec: public MatBase<T, 1> {
 public:
  using value_type = T;
    static constexpr size_t order = MatBase<T, 1>::order;

  Vec() = default;
  virtual ~Vec() = default;

  virtual value_type& operator[](size_t i) = 0;

  virtual const value_type& operator[](size_t i) const = 0;

  virtual value_type& operator()(const Pos<1>& pos) {
    return this->operator[](pos());
  }

  virtual const value_type& operator()(const Pos<1>& pos) const {
    return this->operator[](pos());
  }
};

// VecContinuous is a vector that is possible go through in memory
// by a loop and pointer arithmetic, and all VecContinuous object
// has a member value_type* Data() that return a pointer to internal
// data
template<typename T>
class VecContinuous: public Vec<T> {

 protected:
  // Iterator class
  template <class Value>
  class Iter;

 public:
  using value_type = T;
  using iter_type = value_type;
  typedef Iter<iter_type> iterator;
  typedef Iter<iter_type const> const_iterator;

  static constexpr size_t order = MatBase<T, 2>::order;

  VecContinuous() = default;
  virtual ~VecContinuous() = default;

  virtual value_type& operator[](size_t i) = 0;

  virtual const value_type& operator[](size_t i) const = 0;

  virtual size_t Size() const noexcept = 0;

  virtual value_type* Data() noexcept = 0;

  virtual const value_type* Data() const noexcept = 0;

  iterator begin() noexcept {
    return iterator(Data());
  }

  const_iterator begin() const noexcept {
    return const_iterator(Data());
  }

  iterator end() noexcept {
    // Add the start of data with the size using pointer aritimetic
    return iterator(Data() + Size());
  }

  const_iterator end() const noexcept {
    return const_iterator(Data() + Size());
  }

 protected:
  template <class Value>
  class Iter
    : public boost::iterator_facade<Iter<Value>
        , T
        , boost::forward_traversal_tag> {
    struct enabler {};

   public:
    Iter(Value* p): p_(p) {}

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
class VecDiscontinuous: public Vec<T> {

 protected:
  // Iterator class
  template <class Ref>
  class Iter;

 public:
  using value_type = T;
  typedef Iter<Derived> iterator;
  typedef Iter<Derived const> const_iterator;

  static constexpr size_t order = MatBase<T, 2>::order;

  VecDiscontinuous() = default;
  virtual ~VecDiscontinuous() = default;

  virtual value_type& operator[](size_t i) = 0;

  virtual const value_type& operator[](size_t i) const = 0;

  // Size must the the numbers of iterator step, for example
  // usually the number of rows
  virtual size_t Size() const noexcept = 0;

  iterator begin() noexcept {
    return iterator(static_cast<Derived&>(*this), static_cast<size_t>(0));
  }

  const_iterator begin() const noexcept {
    return const_iterator(static_cast<Derived&>(*this), 0);
  }

  iterator end() noexcept {
    return iterator(static_cast<Derived&>(*this), Size());
  }

  const_iterator end() const noexcept {
    return const_iterator(static_cast<Derived&>(*this), Size());
  }

 protected:
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

    value_type& dereference() const { return ref_[pos_]; }

    Ref& ref_;
    size_t pos_;
  };

};


// MatIter model a matrix with iterator
template<class T, class Derived>
class MatIter: Mat2d<T> {

 protected:
  // Iterator class
  template <class Ref>
  class Iter;

 public:
  using value_type = T;
  typedef Iter<Derived> iterator;
  typedef Iter<Derived const> const_iterator;

  static constexpr size_t order = MatBase<T, 2>::order;

  MatIter() = default;
  virtual ~MatIter() = default;

  virtual const value_type& operator()(size_t x, size_t y) const = 0;

  virtual value_type& operator()(size_t x, size_t y) = 0;

  // operator[] on matrix must return i row or the i col
  // using some appropriated derivated vec class
  virtual value_type& operator[](size_t i) = 0;

  // Size must the the numbers of iterator step, for example
  // usually the number of rows
  virtual size_t SizeIter() const noexcept = 0;

  iterator begin() noexcept {
    return iterator(static_cast<Derived&>(*this), static_cast<size_t>(0));
  }

  const_iterator begin() const noexcept {
    return const_iterator(static_cast<Derived&>(*this), 0);
  }

  iterator end() noexcept {
    return iterator(static_cast<Derived&>(*this), SizeIter());
  }

  const_iterator end() const noexcept {
    return const_iterator(static_cast<Derived&>(*this), SizeIter());
  }

 protected:
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

    value_type& dereference() const { return ref_[pos_]; }

    Ref& ref_;
    size_t pos_;
  };
};

template<class T, class Iter, class ConstIter>
class MatRating: Mat2d<T> {
 public:
  using value_type = T;
  typedef Iter iterator;
  typedef ConstIter const_iterator;

  static constexpr size_t order = MatBase<T, 2>::order;

  MatRating() = default;
  virtual ~MatRating() = default;

  virtual const value_type& operator()(size_t x, size_t y) const = 0;

  virtual value_type& operator()(size_t x, size_t y) = 0;

  virtual void operator()(value_type v, size_t x, size_t y) = 0;

  virtual iterator begin() noexcept = 0;

  virtual const_iterator begin() const noexcept = 0;

  virtual iterator end() noexcept = 0;

  virtual const_iterator end() const noexcept = 0;
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

  virtual const value_type& operator()(size_t x, size_t y) const = 0;

  virtual value_type& operator()(size_t x, size_t y) = 0;

  virtual size_t NumElements() const = 0;

  virtual size_t NumElementsLine(size_t i) const = 0;

  virtual size_t NumElementsCol(size_t i) const = 0;
};

}
#endif //__ERISED_MATRIX_BASE__
