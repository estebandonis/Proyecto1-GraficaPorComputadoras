#pragma once

#include <iostream>
#include <cmath>
#include <random>

#include "color.h"
#include "uniform.h"
#include "fragment.h"
#include "FastNoiseLite.h"

Vertex vertexShader(const Vertex& vertex, const Uniform& uniforms) {

  glm::vec4 clipSpaceVertex = uniforms.projection * uniforms.view * uniforms.model * glm::vec4(vertex.position, 1.0f);

  glm::vec3 ndcVertex = glm::vec3(clipSpaceVertex) / clipSpaceVertex.w;

  glm::vec4 screenVertex = uniforms.viewport * glm::vec4(ndcVertex, 1.0f);

  glm::vec3 transformedNormal = glm::mat3(uniforms.model) * vertex.normal;
  transformedNormal = glm::normalize(transformedNormal);

  return Vertex{
    glm::vec3(screenVertex),
    transformedNormal,
    vertex.position
  };
};

Fragment fragmentShader(Fragment& fragment) {
  fragment.color = fragment.color * fragment.intensity;

  return fragment;
};

float rand(glm:: vec3 co) {
    return glm::fract(sin(glm::dot(co, glm::vec3(12.9898, 78.233, 54.53))) * 43758.5453);
}

float random_float_in_range(float min, float max) {
    float random_number = rand();
    float range = max - min;
    return random_number / (float)RAND_MAX * range + min;
}

Fragment fragmentShader1(Fragment& fragment) {
    Color color;

    glm::vec3 baseColor = glm::vec3(232, 174, 104);

    float stripePattern = glm::abs(glm::cos(fragment.original.y * 12.0f)) * 20.0f;
    float stripePattern2 = glm::abs(glm::sin(fragment.original.y * 20.0f)) * 50.0f;

    glm::vec3 tmpColor = baseColor + stripePattern + stripePattern2;

    // Introduce some pseudo-random noise into the equation for a more realistic look
    float noise = rand(fragment.original);
    tmpColor += noise * 0.1f;


    glm::vec3 gray = glm::vec3(255, 255, 255);
    glm::vec3 spots1 = glm::vec3(247, 230, 214);
    glm::vec3 spots2 = glm::vec3(225, 145, 104);
    glm::vec3 spots3 = glm::vec3(162, 122, 94);

    glm:: vec2 uv = glm::vec2(fragment.original.x, fragment.original.y);

    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    float ox = 900.0f;
    float oy = 10.0f;
    float zoom = 200.0f;

    float noiseValue = noiseGenerator.GetNoise((uv.x + ox) * zoom, (uv.y + oy) * zoom);

    if (noiseValue > 0.8f) {
        tmpColor = spots1;
    }

    float ox1 = 800.0f;
    float oy1 = 900.0f;
    float zoom1 = 300.0f;

    float noiseValue1 = noiseGenerator.GetNoise((uv.x + ox1) * zoom1, (uv.y + oy1) * zoom1);

    if (noiseValue1 > 0.8f) {
        tmpColor = spots2;
    }

    float ox2 = 4300.0f;
    float oy2 = 1300.0f;
    float zoom2 = 200.0f;

    float noiseValue2 = noiseGenerator.GetNoise((uv.x + ox2) * zoom2, (uv.y + oy2) * zoom2);

    if (noiseValue2 > 0.8f) {
        tmpColor = spots3;
    }

    float oxl = 200.0f;
    float oyl = 5600.0f;
    float random_float_L = random_float_in_range(70, 100);
    float zooml = random_float_L;

    float noiseValueL = noiseGenerator.GetNoise((uv.x + oxl) * zooml, (uv.y + oyl* zooml));

    if (noiseValueL > 0.9f) {
        tmpColor = gray;
    }

    color = Color(static_cast<int>(tmpColor.x), static_cast<int>(tmpColor.y), static_cast<int>(tmpColor.z));

    fragment.color = color * fragment.intensity;

    return fragment;
};