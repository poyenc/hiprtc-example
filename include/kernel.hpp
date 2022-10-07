#pragma once

#include <hip/hip_runtime.h>

namespace poyenc {
template <typename DataType>
struct Kernel {
  virtual ~Kernel() = default;

  virtual void run(hipDeviceptr_t input, std::size_t length, hipDeviceptr_t output) = 0;
};
}  // namespace poyenc
