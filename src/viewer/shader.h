#pragma once
#include <shaderc/shaderc.h>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gpu.h>
#include <shaderc/shaderc.hpp>
#include <spdlog/spdlog.h>
#include <vector>

namespace ntg::viz {
enum Shader_Type { SPIRV_VERTEX, SPIRV_FRAGMENT };

class Shader {
private:
  SDL_GPUDevice *device;
  const char *name;
  SDL_GPUShader *shader = nullptr;

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

  Shader(SDL_GPUDevice *device) { this->device = device; }

  SDL_GPUShader *getShader() { return this->shader; }
  bool loadShaderFromFile(const char *path, const char *name, Shader_Type type,
                          const ShaderAttribs *attribs);

  ~Shader() {
    if (this->shader != nullptr) {
      SDL_ReleaseGPUShader(this->device, this->shader);
    }
  }
};
} // namespace ntg::viz
