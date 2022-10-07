namespace poyenc {

struct ByPass final {
  template <typename T>
  __device__ T operator()(T value) const {
    return value;
  }
};

struct Activate final {
  template <typename T>
  __device__ T operator()(T value) const {
    return (0 < value ? value : 0);
  }
};

// FIXME: hiprtc doesn't support non-type template argument for now, to mimic passing
//        std::size_t to this template, LPerBlock should be specialization of std::integral_constant<>
template <typename DataType, typename LPerBlock, typename UnaryFunction>
__global__ void kernel(DataType *input, std::size_t length, DataType *output) {
  for (std::size_t idx = 0; idx < length; ++idx) {
    *output++ = UnaryFunction{}(*input++);
  }
}
}  // namespace poyenc
