#pragma once

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
