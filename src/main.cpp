#include "main.h"
#include <iostream>
#include <ctime>

int main() {
    // Window setup
    Window window(1500, 1000, "Night Forest");
    if (!window.isOpen()) return -1;

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);

    // Shaders
    Shader terrainShader("../src/shaders/terrain.vert", "../src/shaders/terrain.frag");
    Shader treeShader("../src/shaders/tree.vert", "../src/shaders/tree.frag");
    Shader fireflyShader("../src/shaders/firefly.vert", "../src/shaders/firefly.frag");

    // Camera
    Camera camera(glm::vec3(0.0f, 5.0f, 50.0f), 0.0f, 0.0f, 20.0f);

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

    // Debug normals
    // for (int i = 0; i < 10 && i < loader.getVertices().size(); i++) {
    //     std::cout << "Normal " << i << ": "
    //               << loader.getVertices()[i].normal.x << ", "
    //               << loader.getVertices()[i].normal.y << ", "
    //               << loader.getVertices()[i].normal.z << std::endl;
    // }

    // Tree mesh
    Mesh treeMesh(loader.getVertices(), loader.getIndices());

    // Tree texture
    Texture treeTexture("../models/branch.png");

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
                 fireflySystem.getFireflies().size() * sizeof(glm::vec3),
                 nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glVertexAttribDivisor(1, 1);

    glBindVertexArray(0);

    // Timing
    float lastTime = static_cast<float>(glfwGetTime());

    // Render loop
    while (window.isOpen()) {
        float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update fireflies
        fireflySystem.update(deltaTime);

        // Update firefly instance positions
        const auto& fireflies = fireflySystem.getFireflies();
        std::vector<glm::vec3> fireflyPositions;
        fireflyPositions.reserve(fireflies.size());
        for (const auto& f : fireflies)
            fireflyPositions.push_back(f.pos);

        glBindBuffer(GL_ARRAY_BUFFER, fireflyInstanceVBO);
        if (!fireflyPositions.empty()) {
            glBufferSubData(GL_ARRAY_BUFFER, 0,
                            fireflyPositions.size() * sizeof(glm::vec3),
                            fireflyPositions.data());
        }

        // Update camera
        camera.calculateProjectionMatrix(window);
        camera.calculateViewMatrix(window, tileMap);

        // Render terrain
        terrainShader.use();
        terrainShader.setMat4("model", glm::mat4(1.0f));
        terrainShader.setMat4("view", camera.getViewMatrix());
        terrainShader.setMat4("projection", camera.getProjectionMatrix());
        tileMap.draw();

        // Render visible fireflies
        fireflyShader.use();
        fireflyShader.setMat4("view", camera.getViewMatrix());
        fireflyShader.setMat4("projection", camera.getProjectionMatrix());
        fireflyShader.setFloat("size", 0.4f);
        fireflyShader.setVec3("color", glm::vec3(1.0f, 0.9f, 0.5f));

        glBindVertexArray(fireflyVAO);
        glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, static_cast<GLsizei>(fireflies.size()));
        glBindVertexArray(0);

        // Trees + lighting
        treeShader.use();

        treeShader.setVec3("lightColor", glm::vec3(1.0f));
        treeShader.setVec3("viewPos", camera.getPosition());

        // Firefly lights
        treeShader.setInt("numLights", static_cast<int>(fireflies.size()));
        for (int i = 0; i < static_cast<int>(fireflies.size()); i++) {
            treeShader.setVec3("lightPos[" + std::to_string(i) + "]", fireflies[i].pos);
            treeShader.setVec3("lightColorArr[" + std::to_string(i) + "]", fireflies[i].color);
            treeShader.setFloat("lightRadius[" + std::to_string(i) + "]", fireflies[i].radius);
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