
#pragma once


#define MAKE_CALLABLE(_CLASS_, _FUNCNAME_)                                   \
struct Testable_##_CLASS_##_##_FUNCNAME_ : public _CLASS_ {                           \
    using _CLASS_::_FUNCNAME_;                                                        \
};                                                                                    \
                                                                                      \
template <typename Base, auto MemberFn>                                               \
struct FunctionInvoker##_CLASS_##_##_FUNCNAME_ final : public Base {                  \
  template <typename BodyArg>                                                         \
  constexpr decltype(auto) operator()(BodyArg&& Body) const & {                       \
    if constexpr (std::is_member_function_pointer_v<decltype(MemberFn)>) {            \
      return std::invoke(MemberFn, static_cast<const Base&>(*this),                   \
                         std::forward<BodyArg>(Body));                                \
    } else {                                                                          \
      return std::invoke(MemberFn, std::forward<BodyArg>(Body));                      \
    }                                                                                 \
  }                                                                                   \
  template <typename BodyArg>                                                         \
  constexpr decltype(auto) operator()(BodyArg&& Body) & {                             \
    if constexpr (std::is_member_function_pointer_v<decltype(MemberFn)>) {            \
      return std::invoke(MemberFn, static_cast<Base&>(*this),                         \
                         std::forward<BodyArg>(Body));                                \
    } else {                                                                          \
      return std::invoke(MemberFn, std::forward<BodyArg>(Body));                      \
    }                                                                                 \
  }                                                                                   \
  template <typename BodyArg>                                                         \
  constexpr decltype(auto) operator()(BodyArg&& Body) && {                            \
    if constexpr (std::is_member_function_pointer_v<decltype(MemberFn)>) {            \
      return std::invoke(MemberFn, std::move(static_cast<Base&>(*this)),              \
                         std::forward<BodyArg>(Body));                                \
    } else {                                                                          \
      return std::invoke(MemberFn, std::forward<BodyArg>(Body));                      \
    }                                                                                 \
  }                                                                                   \
};                                                                                    \
                                                                                      \
inline constexpr FunctionInvoker##_CLASS_##_##_FUNCNAME_<                             \
    Testable_##_CLASS_##_##_FUNCNAME_,                                                \
    &Testable_##_CLASS_##_##_FUNCNAME_::_FUNCNAME_>                                   \
    Callable##_FUNCNAME_{};
