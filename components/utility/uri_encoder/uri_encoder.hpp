#pragma once

#include <string_view>
#include <string>

namespace util
{

std::string uri_encode(const std::string_view uri_string);
std::string uri_decode(const std::string_view uri_string);

} // namespace util
