#include "window.h"

Window::Window(int width, int height, const char* title){
    if(!glfwInit()){
        std::cerr << "Failed to initialize GLFW" << std::endl;
        window = nullptr;
        return;
    }


    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if(!window){
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        window = nullptr;
        return;
    }
    glfwMakeContextCurrent(window);
    
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cerr << "Failed to initialize GLAD" << std::endl;
        window = nullptr;
        return;
    }
    glEnable(GL_DEPTH_TEST);
}

Window::~Window(){
    if(window){
        glfwDestroyWindow(window);
        glfwTerminate();
    }
}

bool Window::isOpen(){
    return window != nullptr && !glfwWindowShouldClose(window);

}

void Window::swapBuffers(){
    if(window){
        glfwSwapBuffers(window);
    }
}

void Window::pollEvents(){
    glfwPollEvents();
}

GLFWwindow* Window::getGLFWwindow() const{
    return window;
}