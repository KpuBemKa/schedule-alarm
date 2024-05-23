#include "uri_encoder.hpp"

#include <algorithm>
#include <array>
#include <span>

#include "esp_log.h"

constexpr std::array<char, 19> reserved_chars = { ' ', '!', '#', '$', '&', '\'', '(', ')', '*', '+',
                                                  ',', '/', ':', ';', '=', '?',  '@', '[', ']' };

namespace util {

constexpr uint8_t right_mask = 0b0000'1111;
constexpr uint8_t left_mask = 0b1111'0000;

constexpr uint8_t char_int_diff = 0x30;

inline char
int_to_char(const uint8_t integer)
{
  return integer + char_int_diff;
}

inline uint8_t
char_to_int(const char character)
{
  return character - char_int_diff;
}

inline std::pair<char, char>
char_to_hex(const char character)
{
  return std::pair{ int_to_char((character & left_mask) >> 4), int_to_char((character & right_mask)) };
}

inline std::array<char, 3>
encode_character(const char character)
{
  const std::pair<char, char> encoded = char_to_hex(character);
  return std::array{ '%', encoded.first, encoded.second };
}

char
decode_character(const std::string_view encoded_char)
{
  return char_to_int(encoded_char[1]) | (char_to_int(encoded_char[0]) << 4);
}

std::string
uri_encode(const std::string_view uri_string)
{
  std::string result;
  result.reserve(uri_string.size());

  for (const char c : uri_string) {
    if (std::find(reserved_chars.begin(), reserved_chars.end(), c) == std::end(reserved_chars)) {
      result.push_back(c);
      continue;
    }

    const std::array<char, 3> encoded = encode_character(c);
    result.append(encoded.begin(), encoded.end());
  }

  return result;
}

std::string
uri_decode(const std::string_view uri_string)
{
  std::string result;
  result.reserve(uri_string.size());

  for (auto it = uri_string.begin(); it != uri_string.end(); it++) {
    if (*it != '%') {
      result.push_back(*it);
      continue;
    }

    result.push_back(decode_character(std::string_view{ it + 1, 2 }));

    it += 2;
  }

  return result;
}

}