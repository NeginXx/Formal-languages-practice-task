#pragma once

#include <cassert>
#include <cstring>
#include <iostream>
#include <string_view>

// Prints ^_______ to console
void PrintUnderline(uint64_t spaces_num, uint64_t len) {
  assert(spaces_num < len);
  assert(len > 0 && len <= 10'000);

  char buf[len + 1];
  memset(buf, 0, len + 1);
  memset(buf, ' ', spaces_num);
  buf[spaces_num] = '^';
  memset(buf + spaces_num + 1, '_', len - spaces_num - 1);

  std::cout << buf << '\n';
}

void ErrorExprMisconstructed(const std::string& expr, uint64_t symbol_to_highlight,
                             const std::string_view& additional_error_info) {
  std::cout << "Error: regular expression is misconstructed" << '\n';
  std::cout << expr << '\n';
  PrintUnderline(symbol_to_highlight, expr.size());
  if (!additional_error_info.empty()) {
    std::cout << additional_error_info << '\n';
  }
}