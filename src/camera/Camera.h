#ifndef CAMERA_H
#define CAMERA_H

#include "eulerAngle.h"
#include "../window.h"
#include "../WorldGen/tileMap.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Window;
class TileMap;

class Camera {
public:
    Camera();
    Camera(glm::vec3 position);
    Camera(glm::vec3 position, float yaw, float pitch, float roll);
    Camera(glm::vec3 position, glm::vec3 direction);
    ~Camera();

    void calculateViewMatrix(const Window& window, const TileMap& tileMap);
    void calculateProjectionMatrix(const Window& window);

    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();

    void setFOV(int fov);
    void initMousePos(double x, double y);

    glm::vec3 getPosition();
    glm::vec3 getFront();

private:
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 velocity;

    float pitch;
    float yaw;
    float roll;

    float speed;        // base movement scale (kept for tuning)
    float sensitivity;  // mouse sensitivity

    float fov;
    float minDistance;
    float maxDistance;

    glm::mat4 view;
    glm::mat4 projection;

    double xPos;
    double yPos;

    // “feel” parameters
    float acceleration;
    float damping;
    float collisionRadius;

    bool collidesWith(const glm::vec3& newPos, const TileMap& tileMap);
};

#endif