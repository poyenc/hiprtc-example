#include "get_kernel.hpp"

#include <hip/hip_runtime.h>
#include <hip/hiprtc.h>

#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>

#include "utility.hpp"

namespace poyenc {
namespace detail {
template <typename T>
inline constexpr std::string_view get_type_name();

/// FIXME: It would be better if we can use something like nvrtcGetTypeName() to get typename from given std::type_info object.
///        Then we can know the typename of a dependent type in templates.
template <>
inline constexpr std::string_view get_type_name<float>() {
  return "float";
}
}  // namespace detail

std::string_view to_string(Action action) {
  switch (action) {
    case Action::ByPass:
      return "ByPass";
    case Action::Activate:
      return "Activate";
  }

  return "<invalid value>";
}

template <typename DataType, std::size_t LPerBlock>
class ConcreteKernel final : public Kernel<DataType> {
  /// FIXME: It would be better if we can use hiprtcGetLoweredName() to get mangled kernel name without
  ///        instantiating templates. Then we can store kernel name in externel storage and retrieve it
  ///        later separately in different processes.
  static std::optional<std::string> mangled_kernel_name;

  static std::string get_module_file_name(std::string_view mangled_kernel_name) { return std::string(mangled_kernel_name) + ".module"; }

  static std::string get_kernel_name(Action action) {
    std::ostringstream kernel_name;

    kernel_name << "poyenc::kernel<" << detail::get_type_name<DataType>() << ", std::integral_constant<int, " << LPerBlock << ">"
                << ", poyenc::" << to_string(action) << ">";

    return kernel_name.str();
  }

  static std::string instantiate_kernel(Action action) {
    static constexpr std::string_view kernel_template_file = "kernel.cu";

    const std::string kernel_template = read_file_content(kernel_template_file);

    hiprtcProgram program;
    hiprtcCreateProgram(&program, kernel_template.data(), kernel_template_file.data(), 0, nullptr, nullptr);

    const std::string kernel_name = get_kernel_name(action);
    hiprtcAddNameExpression(program, kernel_name.data());

    hipDeviceProp_t properties;
    hipGetDeviceProperties(&properties, 0 /* use device=0 */);
    std::string sarg = std::string("--gpu-architecture=") + properties.gcnArchName;
    const char* options[] = {sarg.c_str()};
    hiprtcResult compile_result = hiprtcCompileProgram(program, 1, options);

    std::size_t log_size;
    hiprtcGetProgramLogSize(program, &log_size);

    if (log_size) {
      std::string log(log_size, '\0');
      hiprtcGetProgramLog(program, log.data());

      std::cerr << log << std::endl;
    }

    if (compile_result != HIPRTC_SUCCESS) {
      throw std::runtime_error("failed to instantiate kernel: " + kernel_name);
    }

    std::size_t module_size;
    hiprtcGetCodeSize(program, &module_size);

    std::string module_content(module_size, '\0');
    hiprtcGetCode(program, module_content.data());

    const char* name;
    hiprtcGetLoweredName(program, kernel_name.data(), &name);
    std::string lowered_kernel_name = name;

    write_file_content(get_module_file_name(lowered_kernel_name), module_content, std::ios::binary);

    hiprtcDestroyProgram(&program);

    return lowered_kernel_name;
  }

 public:
  explicit ConcreteKernel(Action action) {
    if (!mangled_kernel_name) {
      mangled_kernel_name = instantiate_kernel(action);
    }
  }

  void run(hipDeviceptr_t input, std::size_t length, hipDeviceptr_t output) override final {
    const std::string module_content = read_file_content(get_module_file_name(*mangled_kernel_name), std::ios::binary);

    hipModule_t module;
    hipModuleLoadData(&module, module_content.data());

    hipFunction_t kernel;
    hipModuleGetFunction(&kernel, module, mangled_kernel_name->data());

    struct {
      hipDeviceptr_t input_;
      std::size_t length_;
      hipDeviceptr_t output_;
    } args{input, length, output};
    auto size = sizeof(args);
    void* config[] = {HIP_LAUNCH_PARAM_BUFFER_POINTER, &args, HIP_LAUNCH_PARAM_BUFFER_SIZE, &size, HIP_LAUNCH_PARAM_END};

    hipModuleLaunchKernel(kernel, 1, 1, 1, 1, 1, 1, 0, nullptr, nullptr, config);

    hipModuleUnload(module);
  }
};

template <typename DataType, std::size_t LPerBlock>
std::optional<std::string> ConcreteKernel<DataType, LPerBlock>::mangled_kernel_name{std::nullopt};

template <>
std::unique_ptr<Kernel<float>> get_kernel(std::size_t length, Action action) {
  if (2048 <= length) {
    return std::make_unique<ConcreteKernel<float, 256>>(action);
  } else if (1024 <= length) {
    return std::make_unique<ConcreteKernel<float, 128>>(action);
  }

  return std::make_unique<ConcreteKernel<float, 64>>(action);
}
}  // namespace poyenc
