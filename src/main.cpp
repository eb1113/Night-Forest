#include "main.h"


int main() {

    Window window(1500, 1000, "Night Forest");
    if(!window.isOpen()){
        return -1;
    }
    // glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //shaders
    Shader shader("../src/shaders/terrain.vert", "../src/shaders/terrain.frag");
    Camera camera(
        glm::vec3(0.0f, 1.8f, 5.0f),
        -360.0f,
        -0.0f,
         45.0f
    );
    // double mx, my;
    // glfwGetCursorPos(window.getGLFWwindow(), &mx, &my);
    // camera.initMousePos(mx,my);
    glEnable(GL_DEPTH_TEST);
    float lastTime = glfwGetTime();
    TileMap tileMap;
    tileMap.generateGrid(150,150,1.0f);
    tileMap.setupBuffers();

    // load obj file
    ObjLoader loader;
    if(!loader.load("../models/rectanglebox.obj")){
        return -1;
    }

    std::vector<float> vertices = loader.getVertices();
    std::vector<unsigned int> indices = loader.getIndices();


    // Main render loop
    while(window.isOpen()){
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        camera.calculateProjectionMatrix(window);
        camera.calculateViewMatrix(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        glm::mat4 model = glm::mat4(1.0f);
        shader.setMat4("model", model);
        shader.setMat4("view", camera.getViewMatrix());
        shader.setMat4("projection", camera.getProjectionMatrix());

        tileMap.draw();
        window.swapBuffers();
        window.pollEvents();
    }
    return 0;
}
