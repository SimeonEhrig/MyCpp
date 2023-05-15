#include <algorithm>
#include <array>
#include <experimental/mdspan>
#include <iostream>
#include <vector>

#include "MdSpanLinearAdapter.hpp"
#include "SimpleSpan.hpp"

// ###########################################################################
// algorithm (transform and reduce)
// ###########################################################################

template <typename TSpan> void iterate_over_all_elements_impl(TSpan span) {
  for (auto i = 0; i < span.size(); ++i) {
    span[i] = 1;
  }
}

/// @brief Iterate over all elements and set it to 1.
/// @tparam TExtents extend of the mdspan
/// @tparam TLayoutPolicy mapping of the mdspan
/// @tparam TAccessorPolicy accessor of the mdspan
/// @param m mdspan with data
template <typename TExtents, typename TLayoutPolicy, typename TAccessorPolicy>
void iterate_over_all_elements(
    std::experimental::mdspan<TExtents, TLayoutPolicy, TAccessorPolicy> m) {
  if constexpr (m.is_always_exhaustive()) {
    std::cout
        << "use optimized way to iterate over all elements (no padding used)\n";
    iterate_over_all_elements_impl(SimpleSpan{m});

  } else {
    std::cout
        << "use adapter way to iterate over all elements (padding used)\n";
    iterate_over_all_elements_impl(MdSpanLinearAdapter{m});
  }
}

template <typename TSpan> int reduce_elements_impl(TSpan span) {
  int sum = 0;
  for (auto i = 0; i < span.size(); ++i) {
    sum += span[i];
  }
  return sum;
}

/// @brief Summarized all elements of mdspan.
/// @tparam TExtents extend of the mdspan
/// @tparam TLayoutPolicy mapping of the mdspan
/// @tparam TAccessorPolicy accessor of the mdspan
/// @param m mdspan with data
template <typename TExtents, typename TLayoutPolicy, typename TAccessorPolicy>
int reduce_elements(
    std::experimental::mdspan<TExtents, TLayoutPolicy, TAccessorPolicy> m) {
  if constexpr (m.is_always_exhaustive()) {
    std::cout
        << "use optimized way to iterate over all elements (no padding used)\n";
    return reduce_elements_impl(SimpleSpan{m});
  } else {
    std::cout
        << "use adapter way to iterate over all elements (padding used)\n";
    return reduce_elements_impl(MdSpanLinearAdapter{m});
  }
}

// ###########################################################################
// main
// ###########################################################################

int main(int argc, char **argv) {
  std::size_t constexpr size_4d = 3;
  std::size_t constexpr size_3d = 2;
  std::size_t constexpr size_2d = 3;
  std::size_t constexpr size_1d = 5;
  std::size_t constexpr total_size = size_4d * size_3d * size_2d * size_1d;

  std::cout << "###########################################################\n"
            << "check index calculation of 4D data struct\n"
            << "###########################################################\n";
  // because the layout of the mdspan is right, it iterate order will be the
  // same like iota() is writing the numbers in the 1D memory.
  std::cout << "\n";

  std::vector<int> data(total_size);
  std::iota(data.begin(), data.end(), 0);
  std::experimental::mdspan md_data4D(
      data.data(), std::experimental::extents<std::size_t, size_4d, size_3d,
                                              size_2d, size_1d>{});
  MdSpanLinearAdapter adapter(md_data4D);

  bool index_check_4d = true;
  for (auto i = 0; i < total_size; ++i) {
    if (data[i] != adapter[i]) {
      index_check_4d = false;
      std::cout << data[i] << " != " << adapter[i] << "\n";
    }
  }
  if (index_check_4d) {
    std::cout << "indices matches\n";
  }

  std::cout << "\n";

  std::cout << "###########################################################\n"
            << "compare implementation v1 with v2 with 4D data struct\n"
            << "###########################################################\n";
  // because the layout of the mdspan is right, it iterate order will be the
  // same like iota() is writing the numbers in the 1D memory.
  std::cout << "\n";

  using md_data4D_type = decltype(md_data4D);

  MdSpanLinearAdapter<md_data4D_type::element_type,
                      md_data4D_type::extents_type, md_data4D_type::layout_type,
                      md_data4D_type::accessor_type, 1>
      adapter_v1(md_data4D);

  MdSpanLinearAdapter<md_data4D_type::element_type,
                      md_data4D_type::extents_type, md_data4D_type::layout_type,
                      md_data4D_type::accessor_type, 2>
      adapter_v2(md_data4D);

  bool index_check_4d_impl_version = true;
  for (auto i = 0; i < total_size; ++i) {
    if (adapter_v1[i] != adapter_v2[i]) {
      index_check_4d_impl_version = false;
      std::cout << adapter_v1[i] << " != " << adapter_v2[i] << "\n";
    }
  }
  if (index_check_4d_impl_version) {
    std::cout
        << "The implementations of linear index calculation v1 and v2 are "
           "equal\n";
  }

  std::cout << "\n";
  std::cout << "###########################################################\n"
            << "set all elements of 2D matrix (without padding) to 1 \n"
            << "###########################################################\n";
  // iterate over all elements of the 2D data structure and set it 1
  std::cout << "\n";

  std::vector<int> data2D(size_2d * size_1d, 0);
  std::experimental::mdspan<
      int, std::experimental::extents<std::size_t, size_2d, size_1d>>
      md_data2D(data2D.data());

  iterate_over_all_elements(md_data2D);

  std::cout << "\n2D Matrix with " << size_2d << " rows and " << size_1d
            << " columns.\nall elements should be 1\n";

  for (auto y = 0; y < size_2d; ++y) {
    for (auto x = 0; x < size_1d; ++x) {
      std::cout << data2D[y * size_1d + x] << " ";
    }
    std::cout << "\n";
  }

  std::cout << "\n";
  std::cout << "###########################################################\n"
            << "set all elements of 2D matrix (with padding) to 1 \n"
            << "###########################################################\n";
  // iterate over all elements of the 2D data structure and set it 1
  // paddings elements should be not affected
  std::cout << "\n";

  std::size_t constexpr size_1d_padding = 2;
  std::vector<int> data2Dpadding(size_2d * (size_1d + size_1d_padding), 0);

  std::array<std::size_t, 2> padding{size_1d + size_1d_padding, 1};
  using Extend_data2Dpadding =
      std::experimental::extents<std::size_t, size_2d, size_1d>;
  std::experimental::layout_stride::mapping<Extend_data2Dpadding> mp{
      Extend_data2Dpadding{}, padding};
  std::experimental::mdspan md_data2Dpadding(data2Dpadding.data(), mp);

  iterate_over_all_elements(md_data2Dpadding);

  std::cout << "\n2D Matrix with " << size_2d << " rows and " << size_1d
            << " columns. There is a padding of " << size_1d_padding
            << " at the end of each row.\n"
            << "All elements should be 1 except the last " << size_1d_padding
            << " elements on each row (should be 0).\n\n";

  for (auto y = 0; y < size_2d; ++y) {
    for (auto x = 0; x < size_1d + size_1d_padding; ++x) {
      std::cout << data2Dpadding[y * (size_1d + size_1d_padding) + x] << " ";
    }
    std::cout << "\n";
  }

  std::cout << "\n";
  std::cout << "###########################################################\n"
            << "reduce all elements of a 2D Matrix without padding \n"
            << "###########################################################\n";
  // Because all elements are 1, the expected sum is the product of column and
  // row size.
  std::cout << "\n";

  if (int sum = reduce_elements(md_data2D); sum != size_2d * size_1d) {
    std::cout << "reduce_elements(md_data2D) failed\n"
              << sum << " != " << size_2d * size_1d << "\n";
  } else {
    std::cout << "reduce_elements(md_data2D) was successful\n";
  }

  std::cout << "\n";

  std::cout << "###########################################################\n"
            << "reduce all elements of a 2D Matrix without padding \n"
            << "###########################################################\n";
  // Because all elements are 1, the expected sum is the product of column and
  // row size. If the algorithm is correct, it ignores the padded elements.
  std::cout << "\n";

  std::cout << "Set all padding elements (" << size_1d_padding
            << " elements on each row) to -2.\n Otherwise the result would be "
               "not affected, if padding elements are also added.\n";
  for (auto y = 0; y < size_2d; ++y) {
    for (auto x = size_1d; x < size_1d + size_1d_padding; ++x) {
      data2Dpadding[y * (size_1d + size_1d_padding) + x] = -2;
    }
  }

  for (auto y = 0; y < size_2d; ++y) {
    for (auto x = 0; x < size_1d + size_1d_padding; ++x) {
      std::cout << data2Dpadding[y * (size_1d + size_1d_padding) + x] << " ";
    }
    std::cout << "\n";
  }

  std::cout << "\n";

  if (int sum = reduce_elements(md_data2Dpadding); sum != size_2d * size_1d) {
    std::cout << "reduce_elements(md_data2Dpadding) failed\n"
              << sum << " != " << size_2d * size_1d << "\n";
  } else {
    std::cout << "reduce_elements(md_data2Dpadding) was successful\n";
  }

  std::cout << "\n";
  std::cout << "###########################################################\n"
            << "reduce all elements of a 4D Matrix without padding \n"
            << "###########################################################\n";
  // Because all elements are 1, the expected sum is the product of column and
  // row size.
  std::cout << "\n";

  std::size_t constexpr n_data4d = total_size - 1;
  std::size_t constexpr expected_sum_data4D = ((n_data4d) * (n_data4d + 1)) / 2;

  if (int sum = reduce_elements(md_data4D); sum != expected_sum_data4D) {
    std::cout << "reduce_elements(md_data4D) failed\n"
              << sum << " != " << expected_sum_data4D << "\n";
  } else {
    std::cout << "reduce_elements(md_data4D) was successful\n";
  }

  std::cout << "\n";

  return 0;
}
