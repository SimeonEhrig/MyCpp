#include <algorithm>
#include <array>
#include <experimental/mdspan>
#include <iostream>
#include <vector>

// ###########################################################################
// adapter
// ###########################################################################

/// @brief Maps linear index to multi dimension index.
/// This method to calculate the multi dimensional index uses the behavior of
/// C++, that integer division is always rounded down.
///
/// stepLenght{n} = D{n-1} * ... * D1
/// stepLenght{1} = 1
///
/// x{n} = Index / stepLenght{n}
/// x{n-1} = (Index - x{n} * stepLenght{n}) / stepLenght{n-1}
/// x{n-k} = (Index - (x{n} * stepLenght{n}) - ... - (x{n-k+1} *
/// stepLenght{n-k+1})) / stepLenght{n-k} x{1} = Index
///
/// @tparam TElement element type of the mdspan
/// @tparam TExtents extend of the mdspan
/// @tparam TLayoutPolicy mapping of the mdspan
/// @tparam TAccessorPolicy accessor of the mdspan
/// @tparam IndexCalculationVersion Implementation version of index calculation
/// algorithm. Can be 1 or 2.
template <typename TElement, typename TExtents, typename TLayoutPolicy,
          typename TAccessorPolicy, int IndexCalculationVersion = 2>
class MdSpanLinearAdapter {
private:
  using mdspan_type = std::experimental::mdspan<TElement, TExtents,
                                                TLayoutPolicy, TAccessorPolicy>;
  // type of the underlying data structure
  using element_type = typename mdspan_type::element_type;
  // index type
  using index_type = typename mdspan_type::index_type;

  mdspan_type m_mdspan;

  /// @brief Depending of the type of the extend (static or dynamic) call the
  /// correct extends function and return the value.
  /// @tparam current_rank Rank where to get the size.
  /// @return Size of the rank.
  template <index_type current_rank> index_type get_extent_helper() {
    if constexpr (mdspan_type::static_extent(current_rank) ==
                  std::experimental::dynamic_extent)
      return m_mdspan.extent(current_rank);
    else {
      return m_mdspan.static_extent(current_rank);
    }
  }

  /// @brief Calculate the step size for the rank (see class documentation).
  /// @tparam final_rank The searched rank.
  /// @tparam current_rank Iterator variable for the algorithm. Should be not
  /// set by the user.
  /// @return Step size for rank `final_rank`.
  template <index_type final_rank,
            index_type current_rank = mdspan_type::rank() - 1>
  index_type calculate_step_size() {
    if constexpr (final_rank == mdspan_type::rank() - 1) {
      return 1;
    } else if constexpr (current_rank > final_rank) {
      return get_extent_helper<current_rank>() *
             calculate_step_size<final_rank, current_rank - 1>();
    } else {
      return 1;
    }
  }

  template <index_type current_rank, index_type final_rank>
  index_type calculate_extend_index_impl(index_type const index) {
    index_type const stepSize = calculate_step_size<current_rank>();
    index_type extend_index = index / stepSize;

    if constexpr (current_rank == final_rank) {
      return extend_index;
    } else {
      return calculate_extend_index_impl<current_rank + 1, final_rank>(
          index - (extend_index * stepSize));
    }
  }

  template <index_type current_rank, typename TTupel>
  auto calculate_extend_index_impl_v2(index_type const index,
                                      TTupel const multi_index_before) {
    index_type const stepSize = calculate_step_size<current_rank>();
    index_type current_multi_index = index / stepSize;
    auto const multi_index =
        std::tuple_cat(multi_index_before, std::tuple{current_multi_index});

    if constexpr (current_rank == (mdspan_type::rank() - 1)) {
      return multi_index;
    } else {
      return calculate_extend_index_impl_v2<current_rank + 1>(
          index - (current_multi_index * stepSize), multi_index);
    }
  }

  /// @brief Creates of tuple of index positions for each extend.
  /// @param index Linear index, which is mapped to the multi dimensional index.
  /// @return Tupel with index positions.
  auto calculate_extend_index_impl_v2(index_type const index) {
    index_type const stepSize = calculate_step_size<0>();
    auto const current_multi_index = index / stepSize;
    std::tuple<index_type> multi_index = {current_multi_index};

    if constexpr ((mdspan_type::rank() - 1) == 0) {
      return multi_index;
    } else {
      return calculate_extend_index_impl_v2<1>(
          index - (current_multi_index * stepSize), multi_index);
    }
  }

  /// @brief Calculate the index position for rank `final_rank`.
  /// @tparam final_rank Rank of the search index.
  /// @param index Linear index.
  /// @return Returns the dimension index of the rank `final_rank` calculated
  /// from the linear `index`.
  template <index_type final_rank>
  index_type calculate_extend_index(index_type const index) {
    return calculate_extend_index_impl<0, final_rank>(index);
  }

  template <index_type... I>
  auto &access_linear_index_impl(index_type const index,
                                 std::index_sequence<I...>) {
#if MDSPAN_USE_BRACKET_OPERATOR
    return m_mdspan[calculate_extend_index<I>(index)...];
#else
    return m_mdspan(calculate_extend_index<I>(index)...);
#endif
  }

  auto &access_linear_index_impl_v2(index_type const index) {
#if MDSPAN_USE_BRACKET_OPERATOR
    return std::apply(
        [this](auto &&...indices) -> element_type & {
          return m_mdspan[indices...];
        },
        calculate_extend_index_impl_v2(index));

#else
    return std::apply(
        [this](auto &&...indices) -> element_type & {
          return m_mdspan(indices...);
        },
        calculate_extend_index_impl_v2(index));
#endif
  }

public:
  MdSpanLinearAdapter(mdspan_type mdspan) : m_mdspan(mdspan) {}

  /// @brief Access element of mdspan with linear index.
  /// @param index Linear index.
  /// @return element at postion index
  element_type &operator[](index_type const index) const {
    static_assert((IndexCalculationVersion > 0 && IndexCalculationVersion < 3),
                  "Unknown IndexCalculationVersion");

    if constexpr (IndexCalculationVersion == 1) {
      return access_linear_index_impl(
          index, std::make_index_sequence<mdspan_type::rank()>{});
    } else if (IndexCalculationVersion == 2) {
      return access_linear_index_impl_v2(index);
    }
  }

  element_type &operator[](index_type const index) {
    static_assert((IndexCalculationVersion > 0 && IndexCalculationVersion < 3),
                  "Unknown IndexCalculationVersion");

    if constexpr (IndexCalculationVersion == 1) {
      return access_linear_index_impl(
          index, std::make_index_sequence<mdspan_type::rank()>{});
    } else if (IndexCalculationVersion == 2) {
      return access_linear_index_impl_v2(index);
    }
  }

  /// @brief Multiplies the extends of all dimensions.
  /// @return Total number of elements
  auto size() const { return m_mdspan.size(); }
};

// ###########################################################################
// simple std::span
// ###########################################################################

/// @brief Because the code is C++ 17, std::span is not available. Implement own
/// version of std::span, which provide all required elements.
/// @tparam TExtents extend of the mdspan
/// @tparam TLayoutPolicy mapping of the mdspan
/// @tparam TAccessorPolicy accessor of the mdspan
template <typename TExtents, typename TLayoutPolicy, typename TAccessorPolicy>
class SimpleSpan {
  using mdspan_type =
      std::experimental::mdspan<TExtents, TLayoutPolicy, TAccessorPolicy>;
  using element_type = typename mdspan_type::element_type;
  using index_type = typename mdspan_type::index_type;
  mdspan_type m_mdspan;

public:
  SimpleSpan(
      std::experimental::mdspan<TExtents, TLayoutPolicy, TAccessorPolicy> m)
      : m_mdspan(m) {}

  element_type &operator[](index_type const index) {
    return m_mdspan.data_handle()[index];
  }

  auto size() const { return m_mdspan.size(); }
};

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
