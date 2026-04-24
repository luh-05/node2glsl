#include "shader.h"
#include <shaderc/shaderc.h>
#include <shaderc/shaderc.hpp>
#include <shaderc/status.h>

std::vector<uint32_t> compileGLSLToSpv(const std::string &source,
                                       shaderc_shader_kind kind,
                                       const char *filename) {
  shaderc::Compiler compiler;
  shaderc::CompileOptions options;

  options.SetOptimizationLevel(shaderc_optimization_level_performance);

  shaderc::SpvCompilationResult module =
      compiler.CompileGlslToSpv(source, kind, filename, options);

  if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
    spdlog::error("Shaderc error: {}", module.GetErrorMessage());
  }
}
