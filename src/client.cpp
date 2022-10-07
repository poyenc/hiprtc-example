#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>

#include "get_kernel.hpp"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <action> " << std::endl;
    std::cerr << "\taction=1: element-wise copy" << std::endl;
    std::cerr << "\taction=2: activation" << std::endl;
    return EXIT_FAILURE;
  }

  using data_type = float;

  static constexpr std::size_t length = 10;

  std::array<data_type, length> hInput, hOutput;
  std::iota(hInput.begin(), hInput.end(), static_cast<data_type>(0) - length / static_cast<data_type>(2));

  hipDeviceptr_t dInput, dOutput;
  hipMalloc((void **)&dInput, sizeof(data_type) * length);
  hipMalloc((void **)&dOutput, sizeof(data_type) * length);

  hipMemcpyHtoD(dInput, hInput.data(), sizeof(data_type) * length);

  auto kernel = poyenc::get_kernel<data_type>(length, static_cast<poyenc::Action>(std::stoi(argv[1])));
  kernel->run(dInput, length, dOutput);

  hipMemcpyDtoH(hOutput.data(), dOutput, sizeof(data_type) * length);

  std::cout << "result: " << std::endl;
  std::copy(hOutput.begin(), hOutput.end(), std::ostream_iterator<data_type>(std::cout, ", "));
  std::cout << std::endl;

  hipFree((void *)dInput);
  hipFree((void *)dOutput);
}
