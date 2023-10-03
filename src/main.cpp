#include <SDL2/SDL.h>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <glm.hpp>
#include <gtx/io.hpp>
#include <gtc/matrix_transform.hpp>
#include <array>
#include <fstream>
#include <sstream>
#include <string>

#include "FastNoiseLite.h"
#include "fragment.h"
#include "uniform.h"
#include "color.h"
#include "shaders.h"
#include "triangle.h"
#include "camera.h"

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

std::array<std::array<float, WINDOW_WIDTH>, WINDOW_HEIGHT> zbuffer;


SDL_Renderer* renderer;

// Declare a global clearColor of type Color
Color clearColor = {0, 0, 0}; // Initially set to black

// Declare a global currentColor of type Color
Color currentColor = {255, 255, 255}; // Initially set to white

// Function to clear the framebuffer with the clearColor
void clear() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);


    for (auto &row : zbuffer) {
        std::fill(row.begin(), row.end(), 99999.0f);
    }
}

void paintSpace(float ox, float oy, float scale) {
    for (int y = 0; y < WINDOW_HEIGHT; y++) {
        for (int x = 0; x < WINDOW_WIDTH; x++) {
            FastNoiseLite noiseGenerator;
            noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

            float noiseValue = noiseGenerator.GetNoise(((x + ox) * scale), ((y + oy) * scale));

            // If the noise value is above a threshold, draw a star
            if (noiseValue > 0.97f) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White
                SDL_RenderDrawPoint(renderer, x, y);
            } else {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }
    }
}

// Function to set a specific pixel in the framebuffer to the currentColor
void point(Fragment f) {
    if (f.z < zbuffer[f.y][f.x]) {
        SDL_SetRenderDrawColor(renderer, f.color.r, f.color.g, f.color.b, f.color.a);
        SDL_RenderDrawPoint(renderer, f.x, f.y);
        zbuffer[f.y][f.x] = f.z;
    }
}

std::vector<std::vector<Vertex>> primitiveAssembly(const std::vector<Vertex>& transformedVertices) {
    std::vector<std::vector<Vertex>> groupedVertices;

    for (int i = 0; i < transformedVertices.size(); i += 3) {
        std::vector<Vertex> vertexGroup;
        vertexGroup.push_back(transformedVertices[i]);
        vertexGroup.push_back(transformedVertices[i+1]);
        vertexGroup.push_back(transformedVertices[i+2]);
        
        groupedVertices.push_back(vertexGroup);
    }

    return groupedVertices;
}

float xLight = 0.0f;
float yLight = 0.0f;
float zLight = -1.0f;

glm::vec3 L = glm::vec3(xLight, yLight, zLight);

void render(std::vector<glm::vec3> VBO, const Uniform& uniforms, const int shader) {
    // 1. Vertex Shader
    // vertex -> transformedVertices
    
    std::vector<Vertex> transformedVertices(VBO.size() / 2);

    for (int i = 0; i < VBO.size(); i+=2) {
        glm::vec3 v = VBO[i];
        glm::vec3 u = VBO[i+1];

        Vertex vertex = {v, u};
        Vertex Vertexshaded = vertexShader(vertex, uniforms);
        transformedVertices.push_back(Vertexshaded);
    }


    // 2. Primitive Assembly
    // transformedVertices -> triangles
    std::vector<std::vector<Vertex>> triangles = primitiveAssembly(transformedVertices);

    // 3. Rasterize
    // triangles -> Fragments
    std::vector<Fragment> fragments;
    for (const std::vector<Vertex>& triangleVertices : triangles) {
        std::vector<Fragment> rasterizedTriangle = triangle(
            triangleVertices[0],
            triangleVertices[1],
            triangleVertices[2],
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            L
        );
        
        fragments.insert(
            fragments.end(),
            rasterizedTriangle.begin(),
            rasterizedTriangle.end()
        );
    }

    // 4. Fragment Shader
    // Fragments -> colors

    for (Fragment fragment : fragments) {
        if (shader == 0){
            point(fragmentShaderNave(fragment));
        } else if (shader == 1) {
            point(fragmentShaderPlanet(fragment));
        } else if (shader == 2) {
            point(fragmentShaderTierra(fragment));
        } else if (shader == 3) {
            point(fragmentShaderPlanetaX(fragment));
        } else if (shader == 4) {
            point(fragmentShaderJupiter(fragment));
        } else if (shader == 5) {
            point(fragmentShaderEstrella(fragment));
        }
    }
}


float x = 0.0f;
float y = 0.0f;
float z = -2.0f;
float zNave = 4.0f;

glm::mat4 createModelMatrixNave() {
    glm::mat4 translation = glm::translate(glm::mat4(1), glm::vec3(x, y, zNave));
    glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(0.04f, 0.04f, 0.04f));

    return translation * scale;
}

float a = 3.14f / 3.0f;
float b = 3.14f / 3.0f;

float orbitRadiusPlanet = 1.0f;

glm::mat4 createModelMatrixPlanet(float deltaTime) {
    float orbitalAngle = glm::radians(a) * deltaTime;
    float xOrbit = orbitRadiusPlanet * glm::cos(orbitalAngle);
    float zOrbit = orbitRadiusPlanet * glm::sin(orbitalAngle);

    glm::mat4 translation = glm::translate(glm::mat4(1), glm::vec3(xOrbit, 0.0f, zOrbit));
    glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(0.1f, 0.1f, 0.1f));
    b += 20;
    glm::mat4 rotation = glm::rotate(glm::mat4(1), glm::radians(b), glm::vec3(0.0f, 1.0f, 0.0f));

    return translation * scale * rotation;
}

float orbitRadiusTierra = 1.5f;

glm::mat4 createModelMatrixTierra(float deltaTime) {
    float orbitalAngle = glm::radians(a) * deltaTime;
    float xOrbit = orbitRadiusTierra * glm::cos(orbitalAngle);
    float zOrbit = orbitRadiusTierra * glm::sin(orbitalAngle);

    glm::mat4 translation = glm::translate(glm::mat4(1), glm::vec3(xOrbit, 0.0f, zOrbit));
    glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(0.2f, 0.2f, 0.2f));
    b += 20;
    glm::mat4 rotation = glm::rotate(glm::mat4(1), glm::radians(b), glm::vec3(0.0f, 1.0f, 0.0f));

    return translation * scale * rotation;
}

float orbitRadiusPlanetaX = 2.3f;

glm::mat4 createModelMatrixPlanetaX(float deltaTime) {
    float orbitalAngle = glm::radians(a) * deltaTime;
    float xOrbit = orbitRadiusPlanetaX * glm::cos(orbitalAngle);
    float zOrbit = orbitRadiusPlanetaX * glm::sin(orbitalAngle);

    glm::mat4 translation = glm::translate(glm::mat4(1), glm::vec3(xOrbit, 0.0f, zOrbit));
    glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(0.3f, 0.3f, 0.3f));
    b += 20;
    glm::mat4 rotation = glm::rotate(glm::mat4(1), glm::radians(b), glm::vec3(0.0f, 1.0f, 0.0f));

    return translation * scale * rotation;
}

float orbitRadiusJupiter = 3.5f;

glm::mat4 createModelMatrixJupiter(float deltaTime) {
    float orbitalAngle = glm::radians(a) * deltaTime;
    float xOrbit = orbitRadiusJupiter * glm::cos(orbitalAngle);
    float zOrbit = orbitRadiusJupiter * glm::sin(orbitalAngle);

    glm::mat4 translation = glm::translate(glm::mat4(1), glm::vec3(xOrbit, 0.0f, zOrbit));
    glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(0.5f, 0.5f, 0.5f));
    b += 20;
    glm::mat4 rotation = glm::rotate(glm::mat4(1), glm::radians(b), glm::vec3(0.0f, 1.0f, 0.0f));

    return translation * scale * rotation;
}

glm::mat4 createModelMatrixEstrella() {
    glm::mat4 translation = glm::translate(glm::mat4(1), glm::vec3(0.0f, 0.0f, 0.0f));
    glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(0.8f, 0.8f, 0.8f));
    a += 3;
    glm::mat4 rotation = glm::rotate(glm::mat4(1), glm::radians(a), glm::vec3(0.0f, 1.0f, 0.0f));

    return translation * scale * rotation;
}

float xCamera = 0.0f;
float yCamera = 0.0f;
float zCamera = 0.0f;

float cameraRotationX = 0.0f;
float cameraRotationY = 0.0f;
float cameraRotationZ = 0.0f;

glm::mat4 createViewMatrix() {

    // Calculate the camera direction based on rotation angles
    glm::vec3 cameraDirection = glm::vec3(
            glm::cos(cameraRotationY) * glm::sin(cameraRotationX),
            glm::sin(cameraRotationY),
            glm::cos(cameraRotationY) * glm::cos(cameraRotationX)
    );

    return glm::lookAt(
        // donde esta
        glm::vec3(x, y, z),
        // hacia adonde mira
        glm::vec3(x + cameraDirection.x, y + cameraDirection.y, z + cameraDirection.z), // Look-at position
        // arriba
        glm::vec3(0, -20, 0)
    );
}

glm::mat4 createProjectionMatrix() {
  float fovInDegrees = 45.0f;
  float aspectRatio = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT);
  float nearClip = 0.1f;
  float farClip = 100.0f;

  return glm::perspective(glm::radians(fovInDegrees), aspectRatio, nearClip, farClip);
}

float xViewport = 1.0f;
float yViewport = 1.0f;

glm::mat4 createViewportMatrix() {
    // Crear la matriz de viewport
    glm::mat4 viewportMatrix = glm::mat4(1.0f);
    viewportMatrix = glm::scale(viewportMatrix, glm::vec3(WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT * 0.5f, 1.0f));
    viewportMatrix = glm::translate(viewportMatrix, glm::vec3(1.0f, 1.0f, 0.0f));

    return viewportMatrix;
}

struct Face {
    std::array<int, 3> vertexIndices;
    std::array<int, 3> normalIndices;
};

bool loadOBJ(const std::string& path, std::vector<glm::vec3>& out_vertices, std::vector<glm::vec3>& out_textures, std::vector<glm::vec3>& out_normals, std::vector<Face>& out_faces)
{
    std::ifstream file(path);
    if (!file)
    {
        std::cout << "Failed to open the file: " << path << std::endl;
        return false;
    }

    std::string line;
    std::istringstream iss;
    std::string lineHeader;
    glm::vec3 vertex;
    Face face;

    while (std::getline(file, line))
    {
        iss.clear();
        iss.str(line);
        iss >> lineHeader;

        if (lineHeader == "v")
        {
            iss >> vertex.x >> vertex.y >> vertex.z;
            out_vertices.push_back(vertex);
        }
        else if (lineHeader == "vn")
        {
            iss >> vertex.x >> vertex.y >> vertex.z;
            out_normals.push_back(vertex);
        }
        else if (lineHeader == "vt")
        {
            iss >> vertex.x >> vertex.y >> vertex.z;
            out_textures.push_back(vertex);
        }
        else if (lineHeader == "f")
        {
            Face face;
            for (int i = 0; i < 3; ++i)
            {
                std::string faceData;
                iss >> faceData;
                
                std::replace(faceData.begin(), faceData.end(), '/', ' ');

                std::istringstream faceDataIss(faceData);
                int temp;
                faceDataIss >> face.vertexIndices[i] >> temp >> face.normalIndices[i];

                face.vertexIndices[i]--;
                face.normalIndices[i]--;
            }

            out_faces.push_back(face);
        }
    }

    return true;
}

// Function that prints the contents of a vector of glm::vec3
auto printVec3Vector = [](const std::vector<glm::vec3>& vec) {
    for (const auto& v : vec) {
        std::cout << v << std::endl;
    }
};


int main() {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow(".obj Renderer", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    bool running = true;
    SDL_Event event;

    std::vector<glm::vec3> verticesNave;
    std::vector<glm::vec3> texturesNave;
    std::vector<glm::vec3> normalsNave;
    std::vector<Face> facesNave;
    std::vector<glm::vec3> vertexBufferObjectNave;

    if (loadOBJ("assets/Nave.obj", verticesNave, texturesNave, normalsNave, facesNave)) {
        // For each face
        for (const auto& face : facesNave)
        {
            for (int i = 0; i < 3; ++i)
            {
                glm::vec3 vertexPosition = verticesNave[face.vertexIndices[i]];
                glm::vec3 vertexNormal = normalsNave[face.normalIndices[i]];

                vertexBufferObjectNave.push_back(vertexPosition);
                vertexBufferObjectNave.push_back(vertexNormal);
            }
        }
    }

    std::vector<glm::vec3> verticesPlaneta;
    std::vector<glm::vec3> texturesPlaneta;
    std::vector<glm::vec3> normalsPlaneta;
    std::vector<Face> facesPlaneta;
    std::vector<glm::vec3> vertexBufferObjectPlaneta;

    if (loadOBJ("assets/sphere.obj", verticesPlaneta, texturesPlaneta, normalsPlaneta, facesPlaneta)) {
        // For each face
        for (const auto& face : facesPlaneta)
        {
            for (int i = 0; i < 3; ++i)
            {
                glm::vec3 vertexPosition = verticesPlaneta[face.vertexIndices[i]];
                glm::vec3 vertexNormal = normalsPlaneta[face.normalIndices[i]];

                vertexBufferObjectPlaneta.push_back(vertexPosition);
                vertexBufferObjectPlaneta.push_back(vertexNormal);
            }
        }
    }

    Uniform uniforms;
    Uniform uniformsPlanet;
    Uniform uniformsTierra;
    Uniform uniformsPlanetaX;
    Uniform uniformsJupiter;
    Uniform uniformsEstrella;

    Uint32 frameStart, frameTime;
    std::string title = "FPS: ";
    int speed = 10;

    float moveX = 200.0f;
    float moveY = 100.0f;
    float scale = 1000.0f;

    while (running) {
        frameStart = SDL_GetTicks();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            // sdl event in which we can move the object
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_a:
                        x -= 1.0f;
                        xCamera -= 1.0f;
                        xLight -= 1.0f;
                        moveX -= 1.0f;
                        break;
                    case SDLK_d:
                        x += 1.0f;
                        xCamera += 1.0f;
                        xLight += 1.0f;
                        moveX += 1.0f;
                        break;
                    case SDLK_w:
                        y += 1.0f;
                        yCamera += 1.0f;
                        yLight += 1.0f;
                        moveY -= 1.0f;
                        break;
                    case SDLK_s:
                        y -= 1.0f;
                        yCamera -= 1.0f;
                        yLight -= 1.0f;
                        moveY += 1.0f;
                        break;
                    case SDLK_r:
                        z += 1.0f;
                        zCamera += 1.0f;
                        zNave += 1.0f;
                        zLight += 1.0f;
                        scale += 100.0f;
                        break;
                    case SDLK_f:
                        z -= 1.0f;
                        zCamera -= 1.0f;
                        zNave -= 1.0f;
                        zLight -= 1.0f;
                        scale -= 100.0f;
                        break;
                    case SDLK_LEFT:
                        cameraRotationX -= glm::radians(1.0f); // Rotate the camera left
                        moveX -= 1.0f;
                        xLight -= 1.0f;
                        break;
                    case SDLK_RIGHT:
                        cameraRotationX += glm::radians(1.0f); // Rotate the camera right
                        moveX += 1.0f;
                        xLight += 1.0f;
                        break;
                    case SDLK_DOWN:
                        cameraRotationY -= glm::radians(1.0f); // Rotate the camera down
                        moveY += 1.0f;
                        yLight -= 1.0f;
                        break;
                    case SDLK_UP:
                        cameraRotationY += glm::radians(1.0f); // Rotate the camera up
                        moveY -= 1.0f;
                        yLight += 1.0f;
                        break;
                }
            }
        }

        clear();
        paintSpace(moveX, moveY, scale);

//        uniforms.model = createModelMatrixNave();
//        uniforms.view = createViewMatrix();
//        uniforms.projection = createProjectionMatrix();
//        uniforms.viewport = createViewportMatrix();
//        // Call our render function
//        render(vertexBufferObjectNave, uniforms, 0);

        uniformsPlanet.model = createModelMatrixPlanet(3.0f);
        uniformsPlanet.view = createViewMatrix();
        uniformsPlanet.projection = createProjectionMatrix();
        uniformsPlanet.viewport = createViewportMatrix();

        render(vertexBufferObjectPlaneta, uniformsPlanet, 1);

        uniformsTierra.model = createModelMatrixTierra(2.0f);
        uniformsTierra.view = createViewMatrix();
        uniformsTierra.projection = createProjectionMatrix();
        uniformsTierra.viewport = createViewportMatrix();

        render(vertexBufferObjectPlaneta, uniformsTierra, 2);

        uniformsPlanetaX.model = createModelMatrixPlanetaX(1.0f);
        uniformsPlanetaX.view = createViewMatrix();
        uniformsPlanetaX.projection = createProjectionMatrix();
        uniformsPlanetaX.viewport = createViewportMatrix();

        render(vertexBufferObjectPlaneta, uniformsPlanetaX, 3);

        uniformsJupiter.model = createModelMatrixJupiter(0.5f);
        uniformsJupiter.view = createViewMatrix();
        uniformsJupiter.projection = createProjectionMatrix();
        uniformsJupiter.viewport = createViewportMatrix();

        render(vertexBufferObjectPlaneta, uniformsJupiter, 4);

        uniformsEstrella.model = createModelMatrixEstrella();
        uniformsEstrella.view = createViewMatrix();
        uniformsEstrella.projection = createProjectionMatrix();
        uniformsEstrella.viewport = createViewportMatrix();

        render(vertexBufferObjectPlaneta, uniformsEstrella, 5);


        // Present the frame buffer to the screen
        SDL_RenderPresent(renderer);

        // Delay to limit the frame rate
        SDL_Delay(1000 / 60);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}