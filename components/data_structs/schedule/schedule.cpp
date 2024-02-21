#include "schedule.hpp"

#include <algorithm>

namespace data {

schedule::schedule() {}

schedule::schedule(std::size_t no_of_rows, const void *data) {
  rows.reserve(no_of_rows);

  std::copy_n(reinterpret_cast<const schedule_row *>(data), no_of_rows, rows.data());

  rows.resize(no_of_rows);
}

} // namespace data