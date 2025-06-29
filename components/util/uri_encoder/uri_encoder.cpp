#include "uri_encoder.hpp"

#include <algorithm>
#include <array>
#include <span>

namespace util {

constexpr std::array<char, 19> reserved_chars = { ' ', '!', '#', '$', '&', '\'', '(', ')', '*', '+',
                                                  ',', '/', ':', ';', '=', '?',  '@', '[', ']' };

constexpr uint8_t right_mask = 0b0000'1111;
constexpr uint8_t left_mask = 0b1111'0000;

constexpr uint8_t char_int_diff = 0x30;

/**
 * @brief Converts an integer to a character by adding a fixed difference.
 * @param integer The integer to convert.
 * @return The corresponding character.
 */
inline char
int_to_char(const uint8_t integer)
{
    return integer + char_int_diff;
}

/**
 * @brief Converts a character to an integer by subtracting a fixed difference.
 * @param character The character to convert.
 * @return The corresponding integer.
 */
inline uint8_t
char_to_int(const char character)
{
    return character - char_int_diff;
}

/**
 * @brief Converts a character to a pair of hexadecimal characters.
 * @param character The character to convert.
 * @return A pair containing the two hexadecimal characters representing the input character.
 */
inline std::pair<char, char>
char_to_hex(const char character)
{
    return std::pair{ int_to_char((character & left_mask) >> 4), int_to_char((character & right_mask)) };
}

/**
 * @brief Encodes a character into a percent-encoded format.
 * @param character The character to encode.
 * @return An array containing the percent-encoded representation of the character.
 */
inline std::array<char, 3>
encode_character(const char character)
{
    const std::pair<char, char> encoded = char_to_hex(character);
    return std::array{ '%', encoded.first, encoded.second };
}

/**
 * @brief Decodes a percent-encoded character back to its original form.
 * @param encoded_char A string view containing the percent-encoded character (two hexadecimal digits) without the preceding `%`.
 * @return The decoded character.
 */
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
        // Check if the character is not in the reserved characters list
        if (std::find(reserved_chars.begin(), reserved_chars.end(), c) == std::end(reserved_chars)) {
            // If the character is not reserved, add it directly to the result
            result.push_back(c);
            continue;
        }

        // If the character is reserved, encode it, and add it to the result
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
        // If the character is not a percent sign, this character is not encoded
        if (*it != '%') {
            result.push_back(*it);
            continue;
        }

        // Otherwise, decode the next two characters
        result.push_back(decode_character(std::string_view{ it + 1, 2 }));

        // And skip the decoded characters
        it += 2;
    }

    return result;
}

}