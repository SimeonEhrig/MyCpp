#pragma once

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
