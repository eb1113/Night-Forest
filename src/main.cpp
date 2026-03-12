
#include "main.h"
#include <iostream>
#include <ctime>

int main() {
    //window set up 
    Window window(1500, 1000, "Night Forest");
    if (!window.isOpen()) return -1;

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f); 

    //shader
    Shader terrainShader("../src/shaders/terrain.vert", "../src/shaders/terrain.frag");
    Shader treeShader("../src/shaders/tree.vert", "../src/shaders/tree.frag");


    Camera camera(glm::vec3(0.0f, 1.8f, 5.0f), 0.0f, 0.0f, 45.0f);

    //terrian start of world build :)
    TileMap tileMap;
    tileMap.generateGrid(150, 150, 1.0f);
    tileMap.setupBuffers();

    //folliage hope this works 
    int seed = static_cast<int>(time(nullptr));
    tileMap.generateFoliage(seed, 1); // 1 type of tree

    std::cout << "Generated " 
              << tileMap.getTrees().size() 
              << " trees and " 
              << tileMap.getShrubs().size() 
              << " shrubs." << std::endl;

    //load tree model
    ObjLoader loader("../models/fig.obj");
    if (!loader.load()) {
        std::cerr << "Failed to load tree OBJ" << std::endl;
        return -1;
    }

    //debugging for normals
    for (int i = 0; i < 10; i++) {
    std::cout << "Normal " << i << ": "
              << loader.getVertices()[i].normal.x << ", "
              << loader.getVertices()[i].normal.y << ", "
              << loader.getVertices()[i].normal.z << std::endl;
    }
    //create mesh from loaded model
    Mesh treeMesh(loader.getVertices(), loader.getIndices());

    // build instance data
    std::vector<TreeInstanceData> instanceData;
    instanceData.reserve(tileMap.getTrees().size());

    for (const TreeInstance& t : tileMap.getTrees()) {
        instanceData.push_back({t.position, t.rotation, t.scale});
    }

    unsigned int instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, instanceData.size() * sizeof(TreeInstanceData), instanceData.data(), GL_STATIC_DRAW);

    //attach instance buffer to tree mesh
    treeMesh.addInstanceBuffer(instanceVBO);

    //rendering loop 
    while (window.isOpen()) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //update the camera 
        camera.calculateProjectionMatrix(window);
        camera.calculateViewMatrix(window, tileMap);

        //rendering terrain 
        terrainShader.use();
        terrainShader.setMat4("model", glm::mat4(1.0f));
        terrainShader.setMat4("view", camera.getViewMatrix());
        terrainShader.setMat4("projection", camera.getProjectionMatrix());
        tileMap.draw();

        //trees!!
        treeShader.use();
        treeShader.setVec3("lightDir", glm::normalize(glm::vec3(-1.0f,-1.0f, -0.3f)));
        treeShader.setVec3("lightColor", glm::vec3(1.0f)); //this coulde be an error but lets see
        treeShader.setVec3("viewPos", camera.getPosition());

        treeShader.setVec3("materialColor", glm::vec3(1.0f,1.0f, 1.0f));
        treeShader.setMat4("view", camera.getViewMatrix());
        treeShader.setMat4("projection", camera.getProjectionMatrix());

        glBindVertexArray(treeMesh.VAO);
        glDrawElementsInstanced(GL_TRIANGLES, treeMesh.indices.size(), GL_UNSIGNED_INT, 0, instanceData.size());
        glBindVertexArray(0);


        
        window.swapBuffers();
        window.pollEvents();
    }

    glfwTerminate();
    return 0;
}
