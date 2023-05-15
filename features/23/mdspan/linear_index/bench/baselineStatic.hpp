#pragma once

#include <benchmark/benchmark.h>
#include <experimental/mdspan>

template <auto size_4D, auto size_3D, auto size_2D, auto size_1D>
static void BM_BASE_LINE_4D_STATIC(benchmark::State &state) {
  std::size_t const total_size = size_4D * size_3D * size_2D * size_1D;

  std::vector<int> data(total_size);
  std::iota(data.begin(), data.end(), 0);
  std::experimental::mdspan md_data4D(
      data.data(), std::experimental::extents<std::size_t, size_4D, size_3D,
                                              size_2D, size_1D>{});

  int sum = 0;

  for (auto _ : state) {
    for (auto i4 = 0; i4 < size_4D; ++i4) {
      for (auto i3 = 0; i3 < size_4D; ++i3) {
        for (auto i2 = 0; i2 < size_4D; ++i2) {
          for (auto i1 = 0; i1 < size_4D; ++i1) {
#if MDSPAN_USE_BRACKET_OPERATOR
            sum += md_data4D[i4, i3, i2, i1];
#else
            sum += md_data4D(i4, i3, i2, i1);

#endif
            benchmark::DoNotOptimize(sum);
          }
        }
      }
    }
  }
}
