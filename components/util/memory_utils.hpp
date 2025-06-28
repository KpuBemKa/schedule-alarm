#pragma once

#include <cstdint>
#include <span>
#include <vector>

namespace util {

template<typename T>
void
AppendAsBytes(std::vector<uint8_t>& buffer, const T value)
{
    buffer.insert(
      buffer.end(), reinterpret_cast<const uint8_t*>(&value), reinterpret_cast<const uint8_t*>(&value) + sizeof(T));
}

template<typename T>
T
ExtractType(const std::span<const uint8_t> buffer)
{
    assert(buffer.size() >= sizeof(T));
    return *reinterpret_cast<const T*>(buffer.data());
}

} // namespace util
