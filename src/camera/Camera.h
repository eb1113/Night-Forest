#ifndef CAMERA_H
#define CAMERA_H

#include "EulerAngle.h"
#include "../Window.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdio.h>

class Camera {
    public:
        Camera(glm::vec3, glm::vec3);
        Camera(glm::vec3, float, float, float);
        Camera(glm::vec3);
        Camera();
        ~Camera();
        void calculateViewMatrix(const Window& window);
        void calculateProjectionMatrix(const Window& window);
        glm::mat4 getViewMatrix();
        glm::mat4 getProjectionMatrix();
        void setFOV(int);
        
    private:
        glm::mat4 view;
        glm::mat4 projection;
        glm::vec3 direction;
        glm::vec3 position;
        
        double xPos;
        double yPos;
        float pitch;
        float yaw;
        float roll;
        float speed;
        float fov;
        float sensitivity;
        float minDistance;
        float maxDistance;


};
#endif