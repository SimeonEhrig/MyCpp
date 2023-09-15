#include <iostream>
#include <numeric>
#include <string>
#include <string_view>
#include <sycl/sycl.hpp>
#include <vector>

// string contains() requires C++ :-/
bool str_contains(std::string_view const word,
                  std::string_view const searched) {
  return word.find(searched) != std::string::npos;
}

int main() {
  int constexpr size = 10;
  std::vector<int> input(size);
  std::iota(input.begin(), input.end(), 1);

  std::vector<sycl::device> devices = sycl::device::get_devices();

  // iterate over all devices
  for (auto const &dev : devices) {
    std::string const dev_name = dev.get_info<sycl::info::device::name>();
    std::cout << "Device: " << dev_name << "\n";

    // If the AMD and Nvidia GPUs are available and we have not compiled for
    // them, it will cause a runtime error when we try to run a kernel on it.
    // Therefore, we skip the acc if we didn't compiled for it.
    if (dev.has(sycl::aspect::gpu)) {
#ifndef ENABLED_NVIDIA
      if (str_contains(dev_name, "NVIDIA")) {
        std::cerr << "No Nvidia support enabled.\n\n";
        continue;
      }
#endif

#ifndef ENABLED_AMD
      if (str_contains(dev_name, "AMD")) {
        std::cerr << "No AMD support enabled.\n\n";
        continue;
      }
#endif
    }

    std::vector<int> output(size, 0);

    sycl::queue queue(dev);

    {
      sycl::buffer inBuf{input}, outBuf{output};
      queue.submit([&](sycl::handler &h) {
        auto in = inBuf.get_access(h, sycl::read_only);
        auto out = outBuf.get_access(h, sycl::write_only);

        h.parallel_for(size,
                       [=](sycl::id<1> const id) { out[id] = in[id] * 2; });
      });
    }
    // print result
    for (auto const v : output) {
      std::cout << v << " ";
    }
    std::cout << "\n\n";
  }
  return 0;
}
