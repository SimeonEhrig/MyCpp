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

/// @brief Generates a 1D data set and a 2D MdSpan. The function iterates over
/// all elements of the data vector and stores the coordinate and the ordering
/// of the access. Afterwards, the result is visualized. The dimension are
/// compile static.
/// @tparam TLayout Needs to be layout_left or layout_right.
/// @tparam y_size Height of the 2D matrix.
/// @tparam x_size Width of the 2D matrix.
template <MdspanLayoutContinuous TLayout, auto y_size, auto x_size>
void fullyLengthStatic2DPrint() {
  std::cout << ">>> print 2D array with full length static extent and layout "
            << get_layout_name<TLayout>() << "\n";

  std::vector<Point2D> data(y_size * x_size);

  // the counter stores the information, in which order a data element was
  // accessed. 0 means never.
  std::size_t counter = 1;
  using FullyStatic2DExtends = stdex::extents<std::size_t, y_size, x_size>;
  stdex::mdspan<Point2D, FullyStatic2DExtends, TLayout> m(
      data.data(), FullyStatic2DExtends{});

  print_extend(m);
  std::cout << "\n";

  // layout specifics:
  // The ordering of the indices is independet of the layout. Means the first
  // index is the row colum position and the second the row position each time.
  //
  // The layout decides, which is the fast index. In the case of a 2D matrix, it
  // means is it faster to iterate along the rows or the columns. If the layout
  // is `layout_right` (typical C++), it is faster two iterate over the rows. If
  // the layout is `layout_left` (typical Fortran), it is faster to iterate over
  // the columns.
  if constexpr (std::is_same_v<TLayout, stdex::layout_right>) {
    for (auto y = 0; y < m.static_extent(0); ++y) {
      for (auto x = 0; x < m.static_extent(1); ++x) {
        Point2D &point_ref = m[y, x];
        point_ref.y = y;
        point_ref.x = x;
        point_ref.counter = counter++;
      }
    }

    // Visualize the fast index as row of a 2D matrix. In the case of
    // `layout_right`. visualize a row as row. For better understanding, see the
    // next case `layout_left`.
    print2D(data, x_size, 3);
  }

  if constexpr (std::is_same_v<TLayout, stdex::layout_left>) {
    for (auto x = 0; x < m.static_extent(1); ++x) {
      for (auto y = 0; y < m.static_extent(0); ++y) {
        Point2D &point_ref = m[y, x];
        point_ref.y = y;
        point_ref.x = x;
        point_ref.counter = counter++;
      }
    }

    // Column is the fast index. Therefore visualize each colum as row. This is
    // required, that the is possible to compare the memory access of
    // `layout_right` with `layout_left`.
    print2D(data, y_size, 3);
  }

  std::cout << "<<<\n";
}

/// Generates a 1D data set and a 2D MdSpan. The function iterates over
/// all elements of the data vector and stores the coordinate and the ordering
/// of the access. Afterwards, the result is visualized. The dimension are
/// runtime depend.
/// @tparam TLayout Needs to be layout_left or layout_right
/// @param y_size Height of the 2D matrix.
/// @param x_size Width of the 2D matrix
template <MdspanLayoutContinuous TLayout>
void fullyLengthDynamic2DPrint(std::size_t y_size, std::size_t x_size) {
  // For detailed documentation, see function fullyLengthStatic2DPrint().

  std::cout << ">>> print 2D array with full length dynamic extent and layout "
            << get_layout_name<TLayout>() << "\n";

  std::vector<Point2D> data(y_size * x_size);

  std::size_t counter = 1;
  using DynamicStatic2DExtends =
      stdex::extents<std::size_t, stdex::dynamic_extent, stdex::dynamic_extent>;

  using mapping = TLayout::template mapping<DynamicStatic2DExtends>;

  mapping mp{DynamicStatic2DExtends{y_size, x_size}};

  stdex::mdspan m(data.data(), mp);

  print_extend(m);
  std::cout << "\n";

  std::cout << "mp.is_always_unique(): " << std::boolalpha
            << mp.is_always_unique() << "\n";
  std::cout << "mp.is_always_strided(): " << std::boolalpha
            << mp.is_always_strided() << "\n";
  std::cout << "mp.is_always_exhaustive(): " << std::boolalpha
            << mp.is_always_exhaustive() << "\n";
  std::cout << "\n";

  if constexpr (std::is_same_v<TLayout, stdex::layout_right>) {
    for (auto y = 0; y < m.extent(0); ++y) {
      for (auto x = 0; x < m.extent(1); ++x) {
        Point2D &point_ref = m[y, x];
        point_ref.y = y;
        point_ref.x = x;
        point_ref.counter = counter++;
      }
    }

    print2D(data, x_size, 3);
  }

  if constexpr (std::is_same_v<TLayout, stdex::layout_left>) {
    for (auto x = 0; x < m.extent(1); ++x) {
      for (auto y = 0; y < m.extent(0); ++y) {
        Point2D &point_ref = m[y, x];
        point_ref.y = y;
        point_ref.x = x;
        point_ref.counter = counter++;
      }
    }

    print2D(data, y_size, 3);
  }

  std::cout << "<<<\n";
}

int main(int argc, char **argv) {
  std::size_t constexpr y_size = 5;
  std::size_t constexpr x_size = 9;

  fullyLengthStatic2DPrint<stdex::layout_right, y_size, x_size>();
  std::cout << "\n";
  fullyLengthStatic2DPrint<stdex::layout_left, y_size, x_size>();
  std::cout << "\n";

  fullyLengthDynamic2DPrint<stdex::layout_right>(y_size, x_size);
  std::cout << "\n";
  fullyLengthDynamic2DPrint<stdex::layout_left>(y_size, x_size);
  std::cout << "\n";

  return 0;
}
