#pragma once

#include <cstddef>
#include <experimental/mdspan>

/// @brief Print the size of the extent and if it is static or dynamic.
/// @tparam TMdspan Type of the MdSpan with extent.
/// @tparam TRank Current Rank position in the extent of the MdSpan.
/// @param m MdSpan with extent.
template <std::size_t TRank, typename TMdspan>
void print_extend(TMdspan const &m) {
  if constexpr (TMdspan::static_extent(TRank - 1) ==
                std::experimental::dynamic_extent) {
    std::cout << "m.extend(" << TRank - 1 << "): " << m.extent(TRank - 1)
              << "\n";
  } else {
    std::cout << "m.static_extent(" << TRank - 1
              << "): " << m.static_extent(TRank - 1) << "\n";
  }
  if constexpr (TRank > 1) {
    print_extend<TRank - 1>(m);
  }
}

template <typename TMdspan> void print_extend(TMdspan const &m) {
  std::size_t constexpr rank = TMdspan::rank();
  if constexpr (TMdspan::static_extent(rank - 1) ==
                std::experimental::dynamic_extent) {
    std::cout << "m.extend(" << rank - 1 << "): " << m.extent(rank - 1) << "\n";
  } else {
    std::cout << "m.static_extent(" << rank - 1
              << "): " << m.static_extent(rank - 1) << "\n";
  }

  if constexpr (rank > 1) {
    print_extend<(rank - 1)>(m);
  }
}

template <typename TLayout>
concept MdspanLayoutContinuous =
    (std::same_as<TLayout, std::experimental::layout_left> ||
     std::same_as<TLayout, std::experimental::layout_right>);

template <typename TLayout>
concept MdspanLayoutStride =
    (std::same_as<TLayout, std::experimental::layout_stride>);

template <typename TLayout>
concept MdspanLayout =
    (MdspanLayoutContinuous<TLayout> || MdspanLayoutStride<TLayout>);

/// @brief Return name of the layout as string.
/// @tparam TLayout Layout type.
/// @return Name.
template <MdspanLayout TLayout> constexpr char const *get_layout_name() {
  if constexpr (std::is_same_v<TLayout, std::experimental::layout_left>) {
    return "left\n";
  } else if (std::is_same_v<TLayout, std::experimental::layout_right>) {
    return "right\n";
  } else if (std::is_same_v<TLayout, std::experimental::layout_stride>) {
    return "stride\n";
  } else {
    return "impossible\n";
  }
}

/// @brief Print vector as 1D data struct.
/// @param vec Data
/// @param indent Ident output with 0 whitespaces.
void print1D(std::ranges::random_access_range auto vec,
             std::size_t const indent = 0) {
  if (indent > 0) {
    std::stringstream ss;
    for (auto i = 0; i < indent; ++i) {
      ss << " ";
    }
    std::cout << ss.str();
  }
  for (auto v : vec) {
    std::cout << v << " ";
  }
  std::cout << std::endl;
}
