#pragma once

#include <vector>

namespace cHip {
void printDevices();
int getNumberDevices();
void compute(int const dev, int const dim, std::vector<int> &output);
} // namespace cHip
