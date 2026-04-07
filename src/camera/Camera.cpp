
#include "camera.h"
#include "../WorldGen/tilemap.h"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>

Camera::Camera(glm::vec3 position, glm::vec3 direction) {
    this->position = position;
    this->direction = glm::normalize(direction);

    xPos = 0.0;
    yPos = 0.0;

   
    pitch = glm::degrees(asin(this->direction.y));
    yaw   = glm::degrees(atan2(this->direction.z, this->direction.x));
    roll  = 0.0f;

    speed       = 0.25f;
    sensitivity = 0.08f;

    fov         = 45.0f;
    minDistance = 0.1f;
    maxDistance = 100.0f;

    velocity        = glm::vec3(0.0f);
    acceleration    = 0.06f;
    damping         = 0.85f;
    collisionRadius = 0.5f;
}

Camera::Camera(glm::vec3 position, float yaw, float pitch, float roll) {
    this->position = position;
    this->yaw      = yaw;
    this->pitch    = pitch;
    this->roll     = roll;

    xPos = 0.0;
    yPos = 0.0;

    speed       = 0.25f;
    sensitivity = 0.08f;

    fov         = 45.0f;
    minDistance = 0.1f;
    maxDistance = 100.0f;

    velocity        = glm::vec3(0.0f);
    acceleration    = 0.06f;
    damping         = 0.85f;
    collisionRadius = 0.5f;

    direction = glm::normalize(glm::vec3(
        cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
        sin(glm::radians(pitch)),
        sin(glm::radians(yaw)) * cos(glm::radians(pitch))
    ));
}

Camera::Camera(glm::vec3 position) {
    this->position = position;
    direction      = glm::vec3(0.0f, 0.0f, -1.0f);

    xPos = 0.0;
    yPos = 0.0;

    pitch = 0.0f;
    yaw   = 90.0f;
    roll  = 0.0f;

    speed       = 0.25f;
    sensitivity = 0.08f;

    fov         = 45.0f;
    minDistance = 0.1f;
    maxDistance = 100.0f;

    velocity        = glm::vec3(0.0f);
    acceleration    = 0.06f;
    damping         = 0.85f;
    collisionRadius = 0.5f;
}

Camera::Camera() {
    position = glm::vec3(0.0f);
    direction = glm::vec3(0.0f, 0.0f, -1.0f);

    xPos = 0.0;
    yPos = 0.0;

    pitch = 45.0f;
    yaw   = 135.0f;
    roll  = 0.0f;

    speed       = 0.25f;
    sensitivity = 0.08f;

    fov         = 45.0f;
    minDistance = 0.1f;
    maxDistance = 100.0f;

    velocity        = glm::vec3(0.0f);
    acceleration    = 0.02f;
    damping         = 0.85f;
    collisionRadius = 0.5f;
}

Camera::~Camera() {}

// simple collision against trees in TileMap
bool Camera::collidesWith(const glm::vec3& newPos, const TileMap& tileMap) {
    const auto& trees = tileMap.getTrees();
    for (const auto& t : trees) {
        float dist = glm::distance(glm::vec2(newPos.x, newPos.z),
                                   glm::vec2(t.position.x, t.position.z));

        float minDist = collisionRadius + t.scale * 0.5f;
        if (dist < minDist)
            return true;
    }
    return false;
}

void Camera::calculateViewMatrix(const Window& window, const TileMap& tileMap) {
    double mouseX, mouseY;
    glfwGetCursorPos(window.getGLFWwindow(), &mouseX, &mouseY);

    double deltaX = mouseX - xPos;
    double deltaY = yPos - mouseY; 

    xPos = mouseX;
    yPos = mouseY;

    yaw   += static_cast<float>(deltaX) * sensitivity;
    pitch += static_cast<float>(deltaY) * sensitivity;

    pitch = glm::clamp(pitch, -89.0f, 89.99f);

    direction = glm::normalize(glm::vec3(
        cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
        sin(glm::radians(pitch)),
        sin(glm::radians(yaw)) * cos(glm::radians(pitch))
    ));

    glm::vec3 right       = glm::normalize(glm::cross(direction, glm::vec3(0,1,0)));
    glm::vec3 flatforward = glm::normalize(glm::vec3(direction.x, 0.0f, direction.z));

    glm::vec3 input(0.0f);

    if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_W) == GLFW_PRESS)
        input += flatforward;
    if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_S) == GLFW_PRESS)
        input -= flatforward;
    if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_A) == GLFW_PRESS)
        input -= right;
    if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_D) == GLFW_PRESS)
        input += right;

    if (glm::length(input) > 0.0f)
        input = glm::normalize(input);

    // acceleration
    velocity += input * acceleration;

    // only damp when not pressing movement keys
    if (glm::length(input) < 0.001f)
        velocity *= damping;

    // move
    glm::vec3 newPos = position + velocity;


    // terrain follow
    float terrainHeight = tileMap.getHeightAt(newPos.x, newPos.z);
    float eyeHeight = 1.8f;
    if (newPos.y < terrainHeight + eyeHeight)
        newPos.y = terrainHeight + eyeHeight;

    if (!collidesWith(newPos, tileMap))
        position = newPos;
    else
        velocity = glm::vec3(0.0f);

    // subtle head bob based on speed
    float speedMag = glm::length(velocity);
    float bob = 0.0f;
    if (speedMag > 0.001f) {
        bob = sin(static_cast<float>(glfwGetTime()) * 10.0f) *
              0.03f * glm::clamp(speedMag * 5.0f, 0.0f, 1.0f);
    }

    glm::vec3 camPos = position;
    camPos.y += bob;

    view = glm::lookAt(camPos, camPos + direction, glm::vec3(0,1,0));
}

void Camera::calculateProjectionMatrix(const Window& window) {
    int width, height;
    glfwGetWindowSize(window.getGLFWwindow(), &width, &height);

    projection = glm::perspective(glm::radians(fov),
                                  static_cast<float>(width) / static_cast<float>(height),
                                  minDistance,
                                  maxDistance);
}

glm::mat4 Camera::getViewMatrix() {
    return view;
}

glm::mat4 Camera::getProjectionMatrix() {
    return projection;
}

void Camera::setFOV(int fov) {
    this->fov = static_cast<float>(fov);
}

void Camera::initMousePos(double x, double y) {
    xPos = x;
    yPos = y;
}

glm::vec3 Camera::getPosition() {
    return position;
}

glm::vec3 Camera::getFront() {
    return direction;
}
