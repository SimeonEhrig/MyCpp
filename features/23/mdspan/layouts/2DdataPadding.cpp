#include <concepts>
#include <cstddef>
#include <experimental/mdspan>
#include <iostream>
#include <ostream>
#include <sstream>

#include "utils.hpp"

namespace stdex = std::experimental;

// stores y and x coordinate
// counter is used to store the ordering of the memory access in a data struct
struct Point2D {
  std::size_t y = 0;
  std::size_t x = 0;
  std::size_t counter = 0;
};

std::ostream &operator<<(std::ostream &os, Point2D const &p) {
  return os << "[" << p.y << "," << p.x << "] = " << p.counter;
}

/// @brief Print a 1D vector in style of a 2D matrix with layout right.
/// @param vec Vector with data.
/// @param slice Row width of the output.
/// @param indent Allows to ident the output by n whitespaces
void print2D(std::ranges::random_access_range auto vec,
             std::integral auto slice, std::size_t const indent = 0) {
  auto const rows = vec.size() / slice;

  std::cout << "slice: " << slice << "\n";
  std::cout << "rows: " << rows << "\n";

  std::stringstream ss;
  for (auto i = 0; i < indent; ++i) {
    ss << " ";
  }
  std::string const indent_str = ss.str();

  for (auto y = 0; y < rows; ++y) {
    std::cout << indent_str;
    for (auto x = 0; x < slice; ++x) {
      std::cout << vec[y * slice + x] << " ";
    }
    std::cout << std::endl;
  }
}

/// @brief Generates a 1D data set and a 2D MdSpan with padding at the end of
/// each row. The function iterates over all elements of the data vector and
/// stores the coordinate and the ordering of the access. Afterwards, the result
/// is visualized. The dimension are compile static.
/// @tparam TLayout Needs to be layout_left or layout_right.
/// @tparam y_size Height of the 2D matrix.
/// @tparam x_size Width of the 2D matrix.
/// @tparam x_padding Add n additional elements on each row as padding.
template <MdspanLayoutContinuous TLayout, auto y_size, auto x_size,
          auto x_padding>
void fullyLengthStatic2DPrint() {
  std::cout << ">>> print 2D array with full length static extent and layout "
            << get_layout_name<TLayout>() << "\n";

  // the counter stores the information, in which order a data element was
  // accessed. 0 means never.
  std::size_t counter = 1;
  using FullyStatic2DExtends = stdex::extents<std::size_t, y_size, x_size>;

  std::vector<Point2D> data(y_size * (x_size + x_padding));

  // The index is calculated as follow: y * padding[0] + x * padding[1]
  // Therefore the padding is defined as "jump with"
  // in 2D: The first value defines the size of each row and where to jump in
  //        memory, when the next row begins.
  //        The second value defines the distance between two elements in a row.
  // in 3D: The first parameter would define the distance between two planes in
  //        depth. If you want to define a padding without padding space in
  //        memory, the values would be {y_size * x_size, x_size, 1}
  std::array<std::size_t, 2> padding{x_size + x_padding, 1};
  stdex::layout_stride::mapping<FullyStatic2DExtends> mp{FullyStatic2DExtends{},
                                                         padding};

  stdex::mdspan<Point2D, FullyStatic2DExtends, stdex::layout_stride> m(
      data.data(), mp);

  print_extend(m);
  std::cout << "\n";

  std::cout << "mp.is_always_unique(): " << std::boolalpha
            << mp.is_always_unique() << "\n";
  std::cout << "mp.is_always_strided(): " << std::boolalpha
            << mp.is_always_strided() << "\n";
  std::cout << "mp.is_always_exhaustive(): " << std::boolalpha
            << mp.is_always_exhaustive() << "\n";
  std::cout << "\n";

  for (auto y = 0; y < m.static_extent(0); ++y) {
    for (auto x = 0; x < m.static_extent(1); ++x) {
      Point2D &point_ref = m[y, x];
      point_ref.y = y;
      point_ref.x = x;
      point_ref.counter = counter++;
    }
  }

  print2D(data, x_size + x_padding, 3);

  std::cout << "<<<\n";
}

int main(int argc, char **argv) {
  std::size_t constexpr y_size = 5;
  std::size_t constexpr x_size = 9;
  std::size_t constexpr x_padding = 2;

  fullyLengthStatic2DPrint<stdex::layout_right, y_size, x_size, x_padding>();

  return 0;
}
