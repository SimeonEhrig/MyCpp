#include <benchmark/benchmark.h>
#include <experimental/mdspan>

#include "MdSpanLinearAdapter.hpp"

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

BENCHMARK(BM_LINEAR_ADAPTER_FULL_STATIC_SIZE_V1);

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

BENCHMARK(BM_LINEAR_ADAPTER_FULL_STATIC_SIZE_V2);

BENCHMARK_MAIN();
