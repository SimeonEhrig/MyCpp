#include "baselineDynamic.hpp"

void BM_BASE_LINE_4D_DYNAMIC(benchmark::State &state, std::size_t const size_4D,
                             std::size_t const size_3D,
                             std::size_t const size_2D,
                             std::size_t const size_1D) {
  std::size_t const total_size = size_4D * size_3D * size_2D * size_1D;

  std::vector<int> data(total_size);
  std::iota(data.begin(), data.end(), 0);
  std::experimental::mdspan md_data4D(
      data.data(),
      std::experimental::extents<std::size_t, std::experimental::dynamic_extent,
                                 std::experimental::dynamic_extent,
                                 std::experimental::dynamic_extent,
                                 std::experimental::dynamic_extent>{
          size_4D, size_3D, size_2D, size_1D});

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
