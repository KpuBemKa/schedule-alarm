#pragma once

#include <string_view>
#include <string>

namespace util
{

/**
 * @brief Encodes a URI string by replacing reserved characters with percent-encoded values.
 * @param uri_string The URI string to encode.
 * @return A new string with reserved characters replaced by their percent-encoded values.
 */
std::string uri_encode(const std::string_view uri_string);

/**
 * @brief Decodes a percent-encoded URI string back to its original form.
 * @param uri_string The percent-encoded URI string to decode.
 * @return A new string with percent-encoded values replaced by their original characters.
 */
std::string uri_decode(const std::string_view uri_string);

} // namespace util
