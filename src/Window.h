#ifndef WINDOW_H
#define WINDOW_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

class Window {
    public:
        Window(int width, int height, const char* title);
        ~Window();
        
        bool isOpen();
        void swapBuffers();
        void pollEvents();
        GLFWwindow* getGLFWwindow() const;

        private:
            GLFWwindow* window;
};
#endif