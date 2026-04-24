#include <shaderc/shaderc.h>
#pragma ONCE

#include <SDL3/SDL_gpu.h>
#include <iostream>
#include <shaderc/shaderc.hpp>
#include <vector>

namespace ntg::viz {
class Shader {
private:
  SDL_GPUShader *shader;

  SDL_GPUShaderCreateInfo *generateShaderCreateInfo();
  std::vector<uint32_t> compileGLSLToSpv(const std::string &source,
                                         shaderc_shader_kind kind,
                                         const char *filename);

public:
  SDL_GPUShader *getShader();
  bool loadShaderFromFile(const char *path);
  bool loadShader(const char *code);
};
} // namespace ntg::viz
