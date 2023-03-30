#include <experimental/mdspan>
#include <iostream>
#include <limits>
#include <sstream>

namespace stdex = std::experimental;

template <typename SizeType, size_t... Extents>
std::string
get_extend_attributes(stdex::extents<SizeType, Extents...> const &extent,
                      std::string_view const name) {
  std::stringstream ss;
  ss << name << ".rank(): " << extent.rank() << "\n";
  ss << name << ".rank_dynamic(): " << extent.rank_dynamic() << "\n";
  ss << name
     << " calculated static ranks: " << extent.rank() - extent.rank_dynamic()
     << "\n";

  for (auto r = 0; r < extent.rank(); ++r) {
    std::string indent = " ";
    for (auto i = 0; i < r; ++i) {
      indent += "  ";
    }

    ss << indent << name << ".extent(" << r << "): " << extent.extent(r)
       << "\n";
    ss << indent << name << ".static_extent(" << r
       << "): " << extent.static_extent(r) << "\n";
    ss << indent << name << ".static_extent(" << r
       << ") == stdex::dynamic_extent : " << std::boolalpha
       << (extent.static_extent(r) == stdex::dynamic_extent) << "\n";
  }

  return ss.str();
}

int main(int argc, char **argv) {
  std::size_t constexpr z_size = 4;
  std::size_t constexpr y_size = 5;
  std::size_t constexpr x_size = 2;

  std::cout
      << "std::numeric_limits<std::size_t>::max() == stdex::dynamic_extent : "
      << std::boolalpha
      << (std::numeric_limits<std::size_t>::max() == stdex::dynamic_extent)
      << std::endl
      << std::endl;

  stdex::extents fullyDynamic3D{z_size, y_size, x_size};
  std::cout << get_extend_attributes(fullyDynamic3D, "fullyDynamic3D")
            << std::endl;

  std::cout << std::endl;

  stdex::extents<std::size_t, z_size, y_size, x_size> fullyStatic3D{};
  std::cout << get_extend_attributes(fullyStatic3D, "fullyStatic3D")
            << std::endl;

  stdex::extents<std::size_t, z_size, std::dynamic_extent, x_size>
      mixedStaticDynamic3D_1{y_size};
  std::cout << get_extend_attributes(mixedStaticDynamic3D_1,
                                     "mixedStaticDynamic3D_1")
            << std::endl;

  stdex::extents<std::size_t, std::dynamic_extent, y_size, std::dynamic_extent>
      mixedStaticDynamic3D_2{z_size, x_size};
  std::cout << get_extend_attributes(mixedStaticDynamic3D_2,
                                     "mixedStaticDynamic3D_2")
            << std::endl;

  stdex::extents<std::size_t, z_size, y_size, std::dynamic_extent>
      mixedStaticDynamic3D_3{x_size};
  std::cout << get_extend_attributes(mixedStaticDynamic3D_3,
                                     "mixedStaticDynamic3D_3")
            << std::endl;

  return 0;
}
