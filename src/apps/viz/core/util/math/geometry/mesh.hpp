#pragma ONCE
#include <vector>
#include <viz/core/util/math/pch.hpp>

namespace ntg::viz {
struct Vertex {
  glm::vec2 pos;
  glm::vec3 color;
};

class Mesh {
public:
  Mesh();
  ~Mesh();

public:
  std::vector<Vertex> vertices;
};
} // namespace ntg::viz
