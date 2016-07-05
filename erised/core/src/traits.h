#ifndef _ERISED_TRAITS_H_
#define _ERISED_TRAITS_H_

#include <type_traits>

namespace erised {

#define GEN_HAS_MEMBER(member)                                                  \
                                                                                \
template <class T>                                                              \
class HasMember_##member {                                                      \
 private:                                                                       \
  struct yes {char x;};                                                         \
  struct no {yes x[2];};                                                        \
                                                                                \
  struct Fallback { int member; };                                              \
  struct Derived : T, Fallback { };                                             \
                                                                                \
  template <class U>                                                            \
  static no& test ( decltype(U::member)* );                                     \
  template <typename U>                                                         \
  static yes& test (U*);                                                        \
                                                                                \
 public:                                                                        \
  static constexpr bool RESULT = sizeof(test<Derived>(nullptr)) == sizeof(yes); \
};                                                                              \
                                                                                \
template <class T>                                                              \
struct has_member_##member                                                      \
: public std::integral_constant<bool, HasMember_##member<T>::RESULT>            \
{                                                                               \
};

GEN_HAS_MEMBER(continuos)

/// is_continuos
template<typename T>
struct is_continuos {
  static constexpr bool value = has_member_continuos<T>::value;
};

template<typename _Tp, std::size_t _Size>
struct is_continuos<_Tp[_Size]>
: public std::true_type { };

template<typename _Tp>
struct is_continuos<_Tp[]>
: public std::true_type { };

}
#endif //_ERISED_TRAITS_H_
