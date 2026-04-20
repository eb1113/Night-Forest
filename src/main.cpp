#include "main.h"
#include <iostream>
#include <ctime>
#include <algorithm>
#include <array>

namespace {
constexpr int kMaxFireflyLights = 12;
constexpr bool kUseDebugShrubMesh = false;
}

int main() {
    // Window setup
    Window window(1920, 1080, "Night Forest");
    if (!window.isOpen()) return -1;
    
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);

    // Shaders
    Shader terrainShader("../src/shaders/terrain.vert", "../src/shaders/terrain.frag");
    Shader treeShader("../src/shaders/tree.vert", "../src/shaders/tree.frag");
    Shader shrubShader("../src/shaders/shrub.vert", "../src/shaders/shrub.frag");
    Shader fireflyShader("../src/shaders/firefly.vert", "../src/shaders/firefly.frag");
    Shader skyShader("../src/shaders/sky.vert", "../src/shaders/sky.frag");

    // Camera
    Camera camera(glm::vec3(0.0f, 5.0f, 50.0f), 0.0f, 1.0f, 20.0f);
    

    // Terrain
    TileMap tileMap;
    tileMap.generateGrid(100, 100, 1.0f);
    tileMap.setupBuffers();

    // Fireflies
    FireflySystem fireflySystem(300);

    // Foliage
    int seed = static_cast<int>(time(nullptr));
    tileMap.generateFoliage(seed, 1);

    
    std::cout << "Generated "
              << tileMap.getTrees().size()
              << " trees and "
              << tileMap.getShrubs().size()
              << " shrubs." << std::endl;

    // Load tree 
    ObjLoader loader("../models/fir.obj");
    if (!loader.load()) {
        std::cerr << "Failed to load tree OBJ" << std::endl;
        return -1;
    }

   //load shrub
    ObjLoader shrubLoader("../models/low-poly.obj");
    if (!shrubLoader.load()) {
        std::cerr << "Failed to load shrub OBJ" << std::endl;
        return -1;
    }

    // Tree mesh
    Mesh treeMesh(loader.getVertices(), loader.getIndices());
    Mesh shrubMesh(shrubLoader.getVertices(), shrubLoader.getIndices());

    // Tree texture and bushes
    Texture treeTexture("../models/branch.png");
    Texture shrubTexture("../models/low-poly-shrub-extracted/textures/oooo_diffuseOriginal.png");
    Texture shrubAlphaTexture("../models/low-poly-shrub-extracted/textures/oooo_diffuseOriginal.png");
    Texture groundTexture("../models/textures/brown_mud_leaves_01_diff_4k.jpg");

    // Tree instance data
    std::vector<TreeInstanceData> instanceData;
    instanceData.reserve(tileMap.getTrees().size());
    for (const TreeInstance& t : tileMap.getTrees()) {
        instanceData.push_back({ t.position, t.rotation, t.scale });
    }
    // Shrub instance data
    std::vector<TreeInstanceData> shrubInstanceData;
    shrubInstanceData.reserve(tileMap.getShrubs().size());
    for (const ShrubInstance& shrub : tileMap.getShrubs()) {
        shrubInstanceData.push_back({ shrub.position, shrub.rotation, shrub.scale * (kUseDebugShrubMesh ? 3.5f : 0.65f) });
    }

    unsigned int instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, instanceData.size() * sizeof(TreeInstanceData),
                 instanceData.data(), GL_STATIC_DRAW);
    treeMesh.addInstanceBuffer(instanceVBO);

    unsigned int shrubInstanceVBO;
    glGenBuffers(1, &shrubInstanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, shrubInstanceVBO);
    glBufferData(GL_ARRAY_BUFFER, shrubInstanceData.size() * sizeof(TreeInstanceData),
                 shrubInstanceData.data(), GL_STATIC_DRAW);
    shrubMesh.addInstanceBuffer(shrubInstanceVBO);

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

    unsigned int skyVAO = 0;
    unsigned int skyVBO = 0;
    const float skyQuadVerts[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f,  1.0f
    };

    glGenVertexArrays(1, &skyVAO);
    glGenBuffers(1, &skyVBO);
    glBindVertexArray(skyVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyQuadVerts), skyQuadVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    const GLint fireflyLightPosLoc = glGetUniformLocation(treeShader.ID, "lightPos");
    const GLint fireflyLightColorLoc = glGetUniformLocation(treeShader.ID, "lightColorArr");
    const GLint fireflyLightRadiusLoc = glGetUniformLocation(treeShader.ID, "lightRadius");
    const GLint shrubFireflyLightPosLoc = glGetUniformLocation(shrubShader.ID, "lightPos");
    const GLint shrubFireflyLightColorLoc = glGetUniformLocation(shrubShader.ID, "lightColorArr");
    const GLint shrubFireflyLightRadiusLoc = glGetUniformLocation(shrubShader.ID, "lightRadius");
    const GLint terrainFireflyLightPosLoc = glGetUniformLocation(terrainShader.ID, "lightPos");
    const GLint terrainFireflyLightColorLoc = glGetUniformLocation(terrainShader.ID, "lightColorArr");
    const GLint terrainFireflyLightRadiusLoc = glGetUniformLocation(terrainShader.ID, "lightRadius");
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

        // Render sky first 
        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);
        skyShader.use();
        skyShader.setMat4("invProjection", glm::inverse(camera.getProjectionMatrix()));
        skyShader.setMat4("invViewRotation", glm::inverse(glm::mat4(glm::mat3(camera.getViewMatrix()))));
        skyShader.setFloat("time", currentTime);
        skyShader.setVec3("moonDirection", glm::normalize(glm::vec3(-0.35f, 0.72f, -0.6f)));
        glBindVertexArray(skyVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);

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

        // Render terrain
        terrainShader.use();
        terrainShader.setMat4("model", glm::mat4(1.0f));
        terrainShader.setMat4("view", camera.getViewMatrix());
        terrainShader.setMat4("projection", camera.getProjectionMatrix());
        terrainShader.setVec3("lightColor", glm::vec3(1.0f));
        terrainShader.setVec3("viewPos", camera.getPosition());
        terrainShader.setVec3("spotPos", camera.getPosition());
        terrainShader.setVec3("spotDir", camera.getFront());
        terrainShader.setFloat("innerCutoff", glm::cos(glm::radians(12.5f)));
        terrainShader.setFloat("outerCutoff", glm::cos(glm::radians(17.5f)));
        terrainShader.setInt("numLights", activeLightCount);
        if (activeLightCount > 0) {
            glUniform3fv(terrainFireflyLightPosLoc, activeLightCount, glm::value_ptr(lightPositions[0]));
            glUniform3fv(terrainFireflyLightColorLoc, activeLightCount, glm::value_ptr(lightColors[0]));
            glUniform1fv(terrainFireflyLightRadiusLoc, activeLightCount, lightRadii.data());
        }
        groundTexture.bind(0);
        terrainShader.setInt("groundTexture", 0);
        tileMap.draw();

        // Render fireflies
        fireflyShader.use();
        fireflyShader.setMat4("view", camera.getViewMatrix());
        fireflyShader.setMat4("projection", camera.getProjectionMatrix());
        fireflyShader.setFloat("size", 0.28f);
        fireflyShader.setVec3("color", glm::vec3(1.0f, 0.9f, 0.5f));
        fireflyShader.setFloat("time", currentTime);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glDepthMask(GL_FALSE);
        glBindVertexArray(fireflyVAO);
        glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, static_cast<GLsizei>(fireflies.size()));
        glBindVertexArray(0);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        treeShader.use();
        treeShader.setVec3("lightColor", glm::vec3(1.0f));
        treeShader.setVec3("viewPos", camera.getPosition());
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
        treeShader.setInt("useAlphaMask", 0);

        glBindVertexArray(treeMesh.VAO);
        glDrawElementsInstanced(GL_TRIANGLES,
                                static_cast<GLsizei>(treeMesh.indices.size()),
                                GL_UNSIGNED_INT, 0,
                                static_cast<GLsizei>(instanceData.size()));
        glBindVertexArray(0);

        if (kUseDebugShrubMesh) {
            shrubShader.use();
            shrubShader.setVec3("lightColor", glm::vec3(1.0f));
            shrubShader.setVec3("viewPos", camera.getPosition());
            shrubShader.setInt("numLights", activeLightCount);
            if (activeLightCount > 0) {
                glUniform3fv(shrubFireflyLightPosLoc, activeLightCount, glm::value_ptr(lightPositions[0]));
                glUniform3fv(shrubFireflyLightColorLoc, activeLightCount, glm::value_ptr(lightColors[0]));
                glUniform1fv(shrubFireflyLightRadiusLoc, activeLightCount, lightRadii.data());
            }
            shrubShader.setVec3("spotPos", camera.getPosition());
            shrubShader.setVec3("spotDir", camera.getFront());
            shrubShader.setFloat("innerCutoff", glm::cos(glm::radians(12.5f)));
            shrubShader.setFloat("outerCutoff", glm::cos(glm::radians(17.5f)));
            shrubShader.setMat4("view", camera.getViewMatrix());
            shrubShader.setMat4("projection", camera.getProjectionMatrix());
            groundTexture.bind(0);
            shrubShader.setInt("shrubTexture", 0);
            groundTexture.bind(1);
            shrubShader.setInt("shrubAlphaTexture", 1);
        } else {
            shrubShader.use();
            shrubShader.setVec3("lightColor", glm::vec3(1.0f));
            shrubShader.setVec3("viewPos", camera.getPosition());
            shrubShader.setInt("numLights", activeLightCount);
            if (activeLightCount > 0) {
                glUniform3fv(shrubFireflyLightPosLoc, activeLightCount, glm::value_ptr(lightPositions[0]));
                glUniform3fv(shrubFireflyLightColorLoc, activeLightCount, glm::value_ptr(lightColors[0]));
                glUniform1fv(shrubFireflyLightRadiusLoc, activeLightCount, lightRadii.data());
            }
            shrubShader.setVec3("spotPos", camera.getPosition());
            shrubShader.setVec3("spotDir", camera.getFront());
            shrubShader.setFloat("innerCutoff", glm::cos(glm::radians(12.5f)));
            shrubShader.setFloat("outerCutoff", glm::cos(glm::radians(17.5f)));
            shrubShader.setMat4("view", camera.getViewMatrix());
            shrubShader.setMat4("projection", camera.getProjectionMatrix());
            shrubTexture.bind(0);
            shrubShader.setInt("shrubTexture", 0);
            shrubAlphaTexture.bind(1);
            shrubShader.setInt("shrubAlphaTexture", 1);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }

        glBindVertexArray(shrubMesh.VAO);
        glDrawElementsInstanced(GL_TRIANGLES,
                                static_cast<GLsizei>(shrubMesh.indices.size()),
                                GL_UNSIGNED_INT, 0,
                                static_cast<GLsizei>(shrubInstanceData.size()));
        glBindVertexArray(0);
        if (!kUseDebugShrubMesh) {
            glDisable(GL_BLEND);
        }

        window.swapBuffers();
        window.pollEvents();
    }

    glfwTerminate();
    return 0;
}
