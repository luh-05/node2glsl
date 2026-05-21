#pragma once
#include <memory>
#include <shaderc/shaderc.h>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gpu.h>
#include <shaderc/shaderc.hpp>
#include <spdlog/spdlog.h>
#include <vector>

namespace ntg::viz {
enum Shader_Type { SPIRV_VERTEX, SPIRV_FRAGMENT };

class Context;
class GPUWrapper;
class Shader {
private:
  std::shared_ptr<Context> context_;
  const char *name;
  struct GPUShaderDeleter {
    void operator()(SDL_GPUShader *shader) const {}
  };
  std::unique_ptr<SDL_GPUShader, GPUShaderDeleter> shader_;

  /**
   * @brief Compiles GLSL to SPIR-V
   * @param source Shader source code
   * @param kind Shader type
   * @param filename The name of the shader for debug output
   */
  std::vector<uint32_t> compileGLSLToSpv(const std::string &source,
                                         shaderc_shader_kind kind,
                                         const char *filename);

public:
  struct ShaderAttribs {
    uint32_t num_samplers;
    uint32_t num_storage_textures;
    uint32_t num_storage_buffers;
    uint32_t num_uniform_buffers;
    uint32_t props;
  };

  Shader(std::shared_ptr<Context> context) : context_(context) {}
  ~Shader();

  SDL_GPUShader *getShader() { return this->shader_.get(); }
  bool loadShaderFromFile(const char *path, const char *name, Shader_Type type,
                          const ShaderAttribs *attribs);
};
} // namespace ntg::viz
