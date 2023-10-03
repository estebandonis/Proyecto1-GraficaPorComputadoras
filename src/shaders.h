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

Fragment fragmentShaderNave(Fragment& fragment) {
  fragment.color = fragment.color * fragment.intensity;

  return fragment;
}

float rand(glm:: vec3 co) {
    return glm::fract(sin(glm::dot(co, glm::vec3(12.9898, 78.233, 54.53))) * 43758.5453);
}

float random_float_in_range(float min, float max) {
    float random_number = rand();
    float range = max - min;
    return random_number / (float)RAND_MAX * range + min;
}

Fragment fragmentShaderJupiter(Fragment& fragment) {
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
}

Fragment fragmentShaderEstrella(Fragment& fragment) {
    Color color;

    // Base color of the sun
    glm::vec3 tmpColor = glm::vec3(244.0f/255.0f, 140.0f/255.0f, 6.0f/255.0f) * glm::vec3(1.0f, 0.0f, 0.0f);

    // Introduce some pseudo-random noise into the equation for a more realistic look
    float noise = rand(fragment.original);

    // Affect the color intensity with the noise
    tmpColor += glm::vec3(noise, noise, noise) * 0.3f;

    // Add a slight gradient from the center to the edge to give a sense of depth
    float distanceFromCenter = glm::length(fragment.original);
    tmpColor *= 1.0f - distanceFromCenter;

    // Add an extra layer of red
    tmpColor += glm::vec3(0.3f * noise, 0.0f, 0.0f);

    // Convert tmpColor to color
    color = Color(tmpColor.x, tmpColor.y, tmpColor.z);

    // Apply glow effect
    float glow = 4.0f + 1.0f * sin(3.0f * noise);
    fragment.color = color * fragment.intensity * glow;

    return fragment;
}

Fragment fragmentShaderTierra(Fragment& fragment) {
    Color color;

    glm::vec3 groundColor = glm::vec3(0.44f, 0.51f, 0.33f);
    glm::vec3 oceanColor = glm::vec3(0.12f, 0.38f, 0.57f);
    glm::vec3 cloudColor = glm::vec3(1.0f, 1.0f, 1.0f);

    glm::vec3 uv = glm::vec3(
            fragment.original.x,
            fragment.original.y,
            fragment.original.z
    );

    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    float ox = 1200.0f;
    float oy = 3000.0f;
    float zoom = 200.0f;

    float noiseValue = noiseGenerator.GetNoise((uv.x + ox) * zoom, (uv.y + oy) * zoom, uv.z * zoom);

    glm::vec3 tmpColor = (noiseValue < 0.5f) ? oceanColor : groundColor;

    float oxc = 3400.0f;
    float oyc = 9800.0f;
    float zoomc = 100.0f;

    float noiseValueC = noiseGenerator.GetNoise((uv.x + oxc) * zoomc, (uv.y + oyc) * zoomc, uv.z * zoomc);

    float random_float = random_float_in_range(0.5, 0.8);

    if (noiseValueC > random_float) {
        tmpColor = cloudColor;
    }

    color = Color(tmpColor.x, tmpColor.y, tmpColor.z);

    fragment.color = color * fragment.intensity;

    return fragment;
}

Fragment fragmentShaderPlanetaX(Fragment& fragment) {
    Color color;
    glm::vec3 greenColor = glm::vec3(0.56f, 0.64f, 0.19f);
    glm::vec3 blueColor = glm::vec3 (0.31f, 0.51f, 0.61f);
    glm::vec3 blackColor = glm::vec3(0.2f, 0.2f, 0.2f);
    glm::vec3 grayColor = glm::vec3(0.8f, 0.8f, 0.8f);

    glm:: vec2 uv = glm::vec2(fragment.original.x, fragment.original.y);

    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    FastNoiseLite noiseGenerator2;
    noiseGenerator2.SetFractalType(FastNoiseLite::FractalType_FBm);

    float ox = 900.0f;
    float oy = 400.0f;
    float zoom = 200.0f;

    float noiseValue = noiseGenerator.GetNoise((uv.x + ox) * zoom, (uv.y + oy) * zoom);

    glm:: vec3 tmpColor = (noiseValue < 0.3f) ? blueColor: greenColor;

    float oxc = 200.0f;
    float oyc = 5600.0f;
    float zoomc = 1000.0f;

    float noiseValueC = noiseGenerator2.GetNoise((uv.x + oxc) * zoomc, (uv.y + oyc) * zoomc);

    float random_float = random_float_in_range(0.0, 0.8);

    if (noiseValueC > random_float) {
        tmpColor = blackColor;
    }


    float oxl = 200.0f;
    float oyl = 5600.0f;
    float random_float_L = random_float_in_range(70, 100);
    float zooml = random_float_L;

    float noiseValueL = noiseGenerator.GetNoise((uv.x + oxl) * zooml, (uv.y + oyl* zooml));

    if (noiseValueL > 0.8f) {
        tmpColor = grayColor;
    }

    color = Color (tmpColor.x, tmpColor.y, tmpColor.z);

    fragment.color = color * fragment.intensity;

    return fragment;
}

Fragment fragmentShaderPlanet(Fragment& fragment) {
    Color color;
    glm::vec3 redColor = glm::vec3(0.88f, 0.22f, 0.08f);
    glm::vec3 rederColor = glm::vec3 (0.98f, 0.10f, 0.08f);
    glm::vec3 blackColor = glm::vec3(0.2f, 0.2f, 0.2f);
    glm::vec3 grayColor = glm::vec3(0.8f, 0.8f, 0.8f);

    glm:: vec2 uv = glm::vec2(fragment.original.x, fragment.original.y);

    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    float ox = 900.0f;
    float oy = 400.0f;
    float zoom = 200.0f;

    float noiseValue = noiseGenerator.GetNoise((uv.x + ox) * zoom, (uv.y + oy) * zoom);

    glm:: vec3 tmpColor = (noiseValue < 0.3f) ? rederColor: redColor;

    float oxc = 200.0f;
    float oyc = 5600.0f;
    float zoomc = 80.0f;

    float noiseValueC = noiseGenerator.GetNoise((uv.x + oxc) * zoomc, (uv.y + oyc) * zoomc);

    float random_float = random_float_in_range(0.2, 0.8);

    if (noiseValueC > random_float) {
        tmpColor = blackColor;
    }

    float oxl = 200.0f;
    float oyl = 5600.0f;
    float random_float_L = random_float_in_range(70, 100);
    float zooml = random_float_L;

    float noiseValueL = noiseGenerator.GetNoise((uv.x + oxl) * zooml, (uv.y + oyl* zooml));

    if (noiseValueL > 0.8f) {
        tmpColor = grayColor;
    }

    color = Color (tmpColor.x, tmpColor.y, tmpColor.z);

    fragment.color = color * fragment.intensity;

    return fragment;
};