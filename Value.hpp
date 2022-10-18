#pragma once

#include <string>
#include <exception>

enum class ValueState {
  undefined,
  legit_value,
  infinity
};

struct ValueException : std::exception {
  const char* what() const noexcept override {
    return "Invalid Value::GetValue call; either infinity or undefined";
  }
};

template <typename T>
struct Value {
  ValueState state = ValueState::undefined;

 private:
  T val_;

 public:
  Value() = delete;
  Value(T val)
  : state(ValueState::legit_value), val_(val) {}

  Value(ValueState state)
  : state(state) {}

  T GetValue() const {
    if (state == ValueState::legit_value) {
      return val_;
    }
    throw ValueException();
  }
};

template <typename T>
Value<T> MaxValue(const Value<T>& left, const Value<T>& right) {
  bool is_undef1 = left.state == ValueState::undefined;
  bool is_undef2 = right.state == ValueState::undefined;
  bool is_inf1 = left.state == ValueState::infinity;
  bool is_inf2 = right.state == ValueState::infinity;

  if (is_undef1 || is_undef2)
    return is_undef1 ? right : left;
  if (is_inf1 || is_inf2)
    return Value<T>{ValueState::infinity};

  return Value<T>{std::max(left.GetValue(), right.GetValue())};
}

template <typename T>
Value<T> PrefValueForDot(const Value<T>& l_pref,
                         const Value<T>& l_entire,
                         const Value<T>& r_pref)
{
  if (r_pref.state == ValueState::undefined ||
      l_entire.state == ValueState::undefined) {
    return l_pref;
  }

  if (l_entire.state == ValueState::infinity ||
      r_pref.state == ValueState::infinity) {
    return Value<T>{ValueState::infinity};
  }

  return MaxValue(l_pref, Value<T>{l_entire.GetValue() + r_pref.GetValue()});
}

template <typename T>
Value<T> EntireValueForDot(const Value<T>& l_entire,
                           const Value<T>& r_entire)
{
  if (l_entire.state == ValueState::undefined ||
      r_entire.state == ValueState::undefined) {
    return Value<T>{ValueState::undefined};
  }

  if (l_entire.state == ValueState::infinity ||
      r_entire.state == ValueState::infinity) {
    return Value<T>{ValueState::infinity};
  }

  return Value<T>{l_entire.GetValue() + r_entire.GetValue()};
}