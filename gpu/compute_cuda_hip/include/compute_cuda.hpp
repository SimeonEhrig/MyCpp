#pragma once

#include <vector>

namespace cCuda {
void printDevices();
int getNumberDevices();
void compute(int const dev, int const dim, std::vector<int> &output);
} // namespace cCuda
