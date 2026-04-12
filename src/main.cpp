#include "main.h"
#include <iostream>
#include <ctime>
#include <algorithm>
#include <array>

namespace {
constexpr int kMaxFireflyLights = 12;
}

int main() {
    // Window setup
    Window window(2000, 1500, "Night Forest");
    if (!window.isOpen()) return -1;

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);

    // Shaders
    Shader terrainShader("../src/shaders/terrain.vert", "../src/shaders/terrain.frag");
    Shader treeShader("../src/shaders/tree.vert", "../src/shaders/tree.frag");
    Shader fireflyShader("../src/shaders/firefly.vert", "../src/shaders/firefly.frag");

    // Camera
    Camera camera(glm::vec3(0.0f, 5.0f, 50.0f), 0.0f, 1.0f, 20.0f);

    // Terrain
    TileMap tileMap;
    tileMap.generateGrid(300, 300, 1.0f);
    tileMap.setupBuffers();

    // Fireflies
    FireflySystem fireflySystem(100);

    // Foliage
    int seed = static_cast<int>(time(nullptr));
    tileMap.generateFoliage(seed, 1);

    std::cout << "Generated "
              << tileMap.getTrees().size()
              << " trees and "
              << tileMap.getShrubs().size()
              << " shrubs." << std::endl;

    // Load tree model
    ObjLoader loader("../models/fir.obj");
    if (!loader.load()) {
        std::cerr << "Failed to load tree OBJ" << std::endl;
        return -1;
    }

    // Tree mesh
    Mesh treeMesh(loader.getVertices(), loader.getIndices());

    // Tree texture
    Texture treeTexture("../models/branch.png");
    Texture groundTexture("../models/textures/brown_mud_leaves_01_diff_4k.jpg");

    // Tree instance data
    std::vector<TreeInstanceData> instanceData;
    instanceData.reserve(tileMap.getTrees().size());
    for (const TreeInstance& t : tileMap.getTrees()) {
        instanceData.push_back({ t.position, t.rotation, t.scale });
    }

    unsigned int instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, instanceData.size() * sizeof(TreeInstanceData),
                 instanceData.data(), GL_STATIC_DRAW);
    treeMesh.addInstanceBuffer(instanceVBO);

    // Firefly quad + instance buffers
    unsigned int fireflyVAO, fireflyVBO, fireflyInstanceVBO;
    float quadVerts[] = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f,  0.5f
    };

    glGenVertexArrays(1, &fireflyVAO);
    glBindVertexArray(fireflyVAO);

    glGenBuffers(1, &fireflyVBO);
    glBindBuffer(GL_ARRAY_BUFFER, fireflyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glGenBuffers(1, &fireflyInstanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, fireflyInstanceVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 fireflySystem.getFireflies().size() * sizeof(Firefly),
                 nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Firefly), (void*)0);
    glVertexAttribDivisor(1, 1);

    glBindVertexArray(0);

    const GLint fireflyLightPosLoc = glGetUniformLocation(treeShader.ID, "lightPos");
    const GLint fireflyLightColorLoc = glGetUniformLocation(treeShader.ID, "lightColorArr");
    const GLint fireflyLightRadiusLoc = glGetUniformLocation(treeShader.ID, "lightRadius");
    std::vector<int> sortedFireflyIndices(fireflySystem.getFireflies().size());

    float lastTime = static_cast<float>(glfwGetTime());

    // Render loop
    while (window.isOpen()) {
        float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update fireflies
        fireflySystem.update(deltaTime);

        const auto& fireflies = fireflySystem.getFireflies();

        glBindBuffer(GL_ARRAY_BUFFER, fireflyInstanceVBO);
        if (!fireflies.empty()) {
            glBufferSubData(GL_ARRAY_BUFFER, 0,
                            fireflies.size() * sizeof(Firefly),
                            fireflies.data());
        }

        // Update camera
        camera.calculateProjectionMatrix(window);
        camera.calculateViewMatrix(window, tileMap);

        // Render terrain
        terrainShader.use();
        terrainShader.setMat4("model", glm::mat4(1.0f));
        terrainShader.setMat4("view", camera.getViewMatrix());
        terrainShader.setMat4("projection", camera.getProjectionMatrix());
        groundTexture.bind(0);
        terrainShader.setInt("groundTexture", 0);
        tileMap.draw();

        // Render fireflies
        fireflyShader.use();
        fireflyShader.setMat4("view", camera.getViewMatrix());
        fireflyShader.setMat4("projection", camera.getProjectionMatrix());
        fireflyShader.setFloat("size", 0.4f);
        fireflyShader.setVec3("color", glm::vec3(1.0f, 0.9f, 0.5f));

        glBindVertexArray(fireflyVAO);
        glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, static_cast<GLsizei>(fireflies.size()));
        glBindVertexArray(0);


        treeShader.use();

        treeShader.setVec3("lightColor", glm::vec3(1.0f));
        treeShader.setVec3("viewPos", camera.getPosition());

        // Firefly lights
        const int activeLightCount = std::min(static_cast<int>(fireflies.size()), kMaxFireflyLights);
        std::array<glm::vec3, kMaxFireflyLights> lightPositions{};
        std::array<glm::vec3, kMaxFireflyLights> lightColors{};
        std::array<float, kMaxFireflyLights> lightRadii{};

        if (activeLightCount > 0) {
            for (int i = 0; i < static_cast<int>(fireflies.size()); ++i) {
                sortedFireflyIndices[i] = i;
            }

            const glm::vec3 cameraPos = camera.getPosition();
            std::partial_sort(
                sortedFireflyIndices.begin(),
                sortedFireflyIndices.begin() + activeLightCount,
                sortedFireflyIndices.begin() + fireflies.size(),
                [&](int lhs, int rhs) {
                    const glm::vec3 lhsOffset = fireflies[lhs].pos - cameraPos;
                    const glm::vec3 rhsOffset = fireflies[rhs].pos - cameraPos;
                    return glm::dot(lhsOffset, lhsOffset) < glm::dot(rhsOffset, rhsOffset);
                });

            for (int i = 0; i < activeLightCount; ++i) {
                const Firefly& firefly = fireflies[sortedFireflyIndices[i]];
                lightPositions[i] = firefly.pos;
                lightColors[i] = firefly.color;
                lightRadii[i] = firefly.radius;
            }
        }

        treeShader.setInt("numLights", activeLightCount);
        if (activeLightCount > 0) {
            glUniform3fv(fireflyLightPosLoc, activeLightCount, glm::value_ptr(lightPositions[0]));
            glUniform3fv(fireflyLightColorLoc, activeLightCount, glm::value_ptr(lightColors[0]));
            glUniform1fv(fireflyLightRadiusLoc, activeLightCount, lightRadii.data());
        }

        // Flashlight
        treeShader.setVec3("spotPos", camera.getPosition());
        treeShader.setVec3("spotDir", camera.getFront());
        treeShader.setFloat("innerCutoff", glm::cos(glm::radians(12.5f)));
        treeShader.setFloat("outerCutoff", glm::cos(glm::radians(17.5f)));
        treeShader.setMat4("model", glm::mat4(1.0f));
        treeShader.setMat4("view", camera.getViewMatrix());
        treeShader.setMat4("projection", camera.getProjectionMatrix());
        treeTexture.bind(0);
        treeShader.setInt("treeTexture", 0);

        glBindVertexArray(treeMesh.VAO);
        glDrawElementsInstanced(GL_TRIANGLES,
                                static_cast<GLsizei>(treeMesh.indices.size()),
                                GL_UNSIGNED_INT, 0,
                                static_cast<GLsizei>(instanceData.size()));
        glBindVertexArray(0);

        window.swapBuffers();
        window.pollEvents();
    }

    glfwTerminate();
    return 0;
}
