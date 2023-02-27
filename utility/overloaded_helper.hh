#ifndef OVERLOADED_HELPER_HH
#define OVERLOADED_HELPER_HH

template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};

template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

#endif  // OVERLOADED_HELPER_HH