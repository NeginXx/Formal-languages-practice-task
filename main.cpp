#include "Value.hpp"
#include "Error.hpp"

struct ExprException : std::exception {
  uint64_t symbol_to_highlight; // points on symbol to be highlighted when printing error to console
  const char* error_message;

  ExprException(int64_t symbol_to_highlight_, const char* error_message_)
  : symbol_to_highlight(symbol_to_highlight_), error_message(error_message_) {}

  const char* what() const noexcept override {
    return "unexpected symbol found while parsing regular expression";
  }
};

struct Answer {
  Value<uint64_t> k_pref;   // max k that language contains word with x^k as pref
  Value<uint64_t> k_entire; // max k that language contains x^k
  int64_t first_unprocessed_symbol;
};

Answer FindAnswer(const std::string& expr, int64_t cur_idx, char x) {
  assert(cur_idx >= 0 && cur_idx < expr.size());
  char sym = expr[cur_idx];
  if ('a' <= sym && sym <= 'z') {
    if (sym == x) {
      return {1, 1, cur_idx - 1};
    }
    return {Value<uint64_t>{ValueState::undefined},
            Value<uint64_t>{ValueState::undefined},
            cur_idx - 1};
  }

  if (sym == '+') {
    if (cur_idx == 0) {
      throw ExprException{cur_idx, "+ expects 2 operands, none were found though"};
    }

    auto ans_left = FindAnswer(expr, cur_idx - 1, x);
    if (ans_left.first_unprocessed_symbol == -1) {
      throw ExprException{cur_idx, "+ expects 2 operands, one was found though"};
    }

    auto ans_right = FindAnswer(expr, ans_left.first_unprocessed_symbol, x);
    return {MaxValue(ans_left.k_pref, ans_right.k_pref),
            MaxValue(ans_left.k_entire, ans_right.k_entire),
            ans_right.first_unprocessed_symbol};
  }

  if (sym == '.') {
    if (cur_idx == 0) {
      throw ExprException{cur_idx, ". expects 2 operands, none were found though"};
    }

    auto ans_left = FindAnswer(expr, cur_idx - 1, x);
    if (ans_left.first_unprocessed_symbol == -1) {
      throw ExprException{cur_idx, ". expects 2 operands, one was found though"};
    }

    auto ans_right = FindAnswer(expr, ans_left.first_unprocessed_symbol, x);
    return {PrefValueForDot(ans_left.k_pref, ans_left.k_entire, ans_right.k_pref),
            EntireValueForDot(ans_left.k_entire, ans_right.k_entire),
            ans_right.first_unprocessed_symbol};
  }

  if (sym == '*') {
    if (cur_idx == 0) {
      throw ExprException{cur_idx, "* expects 1 operand, none were found though"};
    }

    auto ans = FindAnswer(expr, cur_idx - 1, x);
    // std::cout << '!' << ans.k_pref.GetValue() << ' ' << ans.k_entire.GetValue() << '!' << '\n';
    if (ans.k_entire.state != ValueState::undefined) {
      if (ans.k_entire.state == ValueState::infinity || ans.k_entire.GetValue() > 0) {
        return {Value<uint64_t>{ValueState::infinity},
                Value<uint64_t>{ValueState::infinity},
                ans.first_unprocessed_symbol};
      }
      return {Value<uint64_t>{0}, Value<uint64_t>{0},
              ans.first_unprocessed_symbol};
    }

    return {Value<uint64_t>{0},
            Value<uint64_t>{0},
            ans.first_unprocessed_symbol};
  }

  throw ExprException{cur_idx, "unknown symbol, can not parse. Stop"};
}

void PrintAnswer(Value<uint64_t> res, char input_sym) {
  std::cout << "Answer: ";
  switch (res.state) {
    case ValueState::undefined:
      std::cout << "UNDEFINED\nno such k exists, that there is a word with " <<
                   input_sym << "^k as prefix\n";
      break;
    case ValueState::infinity:
      std::cout << "INFINITY\nevery natural number k satisfies \"there is a word with " <<
                   input_sym << "^k as prefix\"\n";
      break;
    case ValueState::legit_value:
      std::cout << res.GetValue() << '\n';
      break;
    default: assert(0);
  }
}

int main() {
  std::string expr;
  char input_sym;
  std::cin >> expr >> input_sym;

  Answer ans{ValueState::undefined, ValueState::undefined, (int64_t)expr.size()};
  try {
    ans = FindAnswer(expr, expr.size() - 1, input_sym);
  } catch (const ExprException& exc) {
    ErrorExprMisconstructed(expr, exc.symbol_to_highlight,
                            exc.error_message);
    return 1;
  }

  if (ans.first_unprocessed_symbol > -1) {
    ErrorExprMisconstructed(expr, ans.first_unprocessed_symbol, "unexpected sequence flow, does not match to any operator");
    return 1;
  }
  PrintAnswer(ans.k_pref, input_sym);
}