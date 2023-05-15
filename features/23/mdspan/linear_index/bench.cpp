#include <benchmark/benchmark.h>
#include <experimental/mdspan>

#include "MdSpanLinearAdapter.hpp"

template <auto CurrentRank, typename TIndexType, std::size_t... Extents>
TIndexType
get_extend_size(std::experimental::extents<TIndexType, Extents...> const &e) {
  if constexpr (std::experimental::extents<
                    TIndexType, Extents...>::static_extent(CurrentRank) ==
                std::experimental::dynamic_extent) {
    return e.extent(CurrentRank);
  } else {
    return e.static_extent(CurrentRank);
  }
}

template <auto CurrentRank, typename TIndexType, std::size_t... Extents>
TIndexType
get_total_size(std::experimental::extents<TIndexType, Extents...> const &e) {
  if constexpr (CurrentRank <
                (std::experimental::extents<TIndexType, Extents...>::rank() -
                 1)) {
    return get_extend_size<CurrentRank>(e) * get_total_size<CurrentRank + 1>(e);
  } else {
    return get_extend_size<CurrentRank>(e);
  }
}

template <typename TIndexType, std::size_t... Extents>
std::size_t
get_total_size(std::experimental::extents<TIndexType, Extents...> const &e) {
  if constexpr (std::experimental::extents<TIndexType, Extents...>::rank() ==
                1) {
    return get_extend_size<0>(e);
  } else {
    return get_extend_size<0>(e) * get_total_size<1>(e);
  }
}

static void BM_LINEAR_ADAPTER_FULL_STATIC_SIZE_V1(benchmark::State &state) {
  std::size_t constexpr size_4d = 3;
  std::size_t constexpr size_3d = 2;
  std::size_t constexpr size_2d = 3;
  std::size_t constexpr size_1d = 5;
  std::size_t constexpr total_size = size_4d * size_3d * size_2d * size_1d;

  std::vector<int> data(total_size);
  std::iota(data.begin(), data.end(), 0);
  std::experimental::mdspan md_data4D(
      data.data(), std::experimental::extents<std::size_t, size_4d, size_3d,
                                              size_2d, size_1d>{});

  using md_data4D_type = decltype(md_data4D);

  MdSpanLinearAdapter<md_data4D_type::element_type,
                      md_data4D_type::extents_type, md_data4D_type::layout_type,
                      md_data4D_type::accessor_type, 1>
      adapter_v1(md_data4D);

  int sum = 0;

  for (auto _ : state) {
    for (auto i = 0; i < total_size; ++i) {
      sum += adapter_v1[i];
      benchmark::DoNotOptimize(sum);
    }
  }
}

static void BM_LINEAR_ADAPTER_FULL_STATIC_SIZE_V2(benchmark::State &state) {
  std::size_t constexpr size_4d = 3;
  std::size_t constexpr size_3d = 2;
  std::size_t constexpr size_2d = 3;
  std::size_t constexpr size_1d = 5;
  std::size_t constexpr total_size = size_4d * size_3d * size_2d * size_1d;

  std::vector<int> data(total_size);
  std::iota(data.begin(), data.end(), 0);
  std::experimental::mdspan md_data4D(
      data.data(), std::experimental::extents<std::size_t, size_4d, size_3d,
                                              size_2d, size_1d>{});

  using md_data4D_type = decltype(md_data4D);

  MdSpanLinearAdapter<md_data4D_type::element_type,
                      md_data4D_type::extents_type, md_data4D_type::layout_type,
                      md_data4D_type::accessor_type, 2>
      adapter_v2(md_data4D);

  int sum = 0;

  for (auto _ : state) {
    for (auto i = 0; i < total_size; ++i) {
      sum += adapter_v2[i];
      benchmark::DoNotOptimize(sum);
    }
  }
}

template <auto ImplementationVersion>
static void BM_LINEAR_ADAPTER_FULL_STATIC_SIZE_Vx(benchmark::State &state) {
  std::size_t constexpr size_4d = 3;
  std::size_t constexpr size_3d = 2;
  std::size_t constexpr size_2d = 3;
  std::size_t constexpr size_1d = 5;
  std::size_t constexpr total_size = size_4d * size_3d * size_2d * size_1d;

  std::vector<int> data(total_size);
  std::iota(data.begin(), data.end(), 0);
  std::experimental::mdspan md_data4D(
      data.data(), std::experimental::extents<std::size_t, size_4d, size_3d,
                                              size_2d, size_1d>{});

  using md_data4D_type = decltype(md_data4D);

  MdSpanLinearAdapter<typename md_data4D_type::element_type,
                      typename md_data4D_type::extents_type,
                      typename md_data4D_type::layout_type,
                      typename md_data4D_type::accessor_type,
                      ImplementationVersion>
      adapter_v1(md_data4D);

  int sum = 0;

  for (auto _ : state) {
    for (auto i = 0; i < total_size; ++i) {
      sum += adapter_v1[i];
      benchmark::DoNotOptimize(sum);
    }
  }
}

template <auto ImplementationVersion, typename TExtend>
static void BM_LINEAR_ADAPTER_GENERIC_EXTEND(benchmark::State &state,
                                             TExtend &&extend) {
  std::size_t const total_size = get_total_size(extend);

  std::vector<int> data(total_size);
  std::iota(data.begin(), data.end(), 0);
  std::experimental::mdspan md_data4D(data.data(), extend);

  using md_data4D_type = decltype(md_data4D);

  MdSpanLinearAdapter<typename md_data4D_type::element_type,
                      typename md_data4D_type::extents_type,
                      typename md_data4D_type::layout_type,
                      typename md_data4D_type::accessor_type,
                      ImplementationVersion>
      adapter_v1(md_data4D);

  int sum = 0;

  for (auto _ : state) {
    for (auto i = 0; i < total_size; ++i) {
      sum += adapter_v1[i];
      benchmark::DoNotOptimize(sum);
    }
  }
}

template <typename TExtend>
static void BM_LINEAR_ADAPTER_GENERIC_EXTEND_V1(benchmark::State &state,
                                                TExtend &&extend) {
  BM_LINEAR_ADAPTER_GENERIC_EXTEND<1>(state, std::move(extend));
}

template <typename TExtend>
static void BM_LINEAR_ADAPTER_GENERIC_EXTEND_V2(benchmark::State &state,
                                                TExtend &&extend) {
  BM_LINEAR_ADAPTER_GENERIC_EXTEND<2>(state, std::move(extend));
}

BENCHMARK(BM_LINEAR_ADAPTER_FULL_STATIC_SIZE_V1);
BENCHMARK(BM_LINEAR_ADAPTER_FULL_STATIC_SIZE_V2);

BENCHMARK(BM_LINEAR_ADAPTER_FULL_STATIC_SIZE_Vx<1>);
BENCHMARK(BM_LINEAR_ADAPTER_FULL_STATIC_SIZE_Vx<2>);

BENCHMARK_CAPTURE(BM_LINEAR_ADAPTER_GENERIC_EXTEND_V1, "with extend<3,2,3,5>{}",
                  std::experimental::extents<std::size_t, 3, 2, 3, 5>{});
BENCHMARK_CAPTURE(BM_LINEAR_ADAPTER_GENERIC_EXTEND_V2, "with extend<3,2,3,5>{}",
                  std::experimental::extents<std::size_t, 3, 2, 3, 5>{});

BENCHMARK_CAPTURE(
    BM_LINEAR_ADAPTER_GENERIC_EXTEND_V1,
    "with extend<dyn, dyn, dyn, dyn>{3,2,3,5}",
    std::experimental::extents<std::size_t, std::experimental::dynamic_extent,
                               std::experimental::dynamic_extent,
                               std::experimental::dynamic_extent,
                               std::experimental::dynamic_extent>{3, 2, 3, 5});
BENCHMARK_CAPTURE(
    BM_LINEAR_ADAPTER_GENERIC_EXTEND_V2,
    "with extend<dyn, dyn, dyn, dyn>{3,2,3,5}",
    std::experimental::extents<std::size_t, std::experimental::dynamic_extent,
                               std::experimental::dynamic_extent,
                               std::experimental::dynamic_extent,
                               std::experimental::dynamic_extent>{3, 2, 3, 5});

BENCHMARK_MAIN();
