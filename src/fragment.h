#pragma once
#include "color.h"
#include <glm.hpp>
#include <cstdint>

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec3 original;
};

struct Fragment {
  uint16_t x;
  uint16_t y;
  double z;
  Color color;
  float intensity;
  glm::vec3 original;
};

struct FragColor {
  Color color;
  double z;
};