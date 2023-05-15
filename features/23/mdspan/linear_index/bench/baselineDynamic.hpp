#pragma once

#include <benchmark/benchmark.h>
#include <experimental/mdspan>

void BM_BASE_LINE_4D_DYNAMIC(benchmark::State &state, std::size_t const size_4D,
                             std::size_t const size_3D,
                             std::size_t const size_2D,
                             std::size_t const size_1D);
