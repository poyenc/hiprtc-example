#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string_view>

#include "kernel.hpp"

namespace poyenc {

enum class Action : std::uint8_t {
  ByPass = 1,
  Activate,
};

std::string_view to_string(Action);

template <typename DataType>
std::unique_ptr<Kernel<DataType>> get_kernel(std::size_t length, Action action = Action::ByPass);
}  // namespace poyenc
