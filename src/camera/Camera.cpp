#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 direction){
    //camera direction and position
    this->position = position;
    this->direction = direction;
    
    //create the mouse x and y
    xPos = 0;
    yPos = 0;

    //create viewing angles
    EulerAngle angles = EulerAngle();
    angles.toAngles(this->direction - this->position);

    this ->pitch = angles.getPitch();
    this ->yaw = angles.getYaw();
    this ->roll = 0;

    fprintf(stdout, "pitch: %f, yaw: %f\n", pitch, yaw);

    //set speed
    speed = 0.25;
    sensitivity = 0.02;

    //create projection variables
    fov = 45.0;
    minDistance = 0.1;
    maxDistance = 100.0;
    
}
Camera::Camera(glm::vec3 position, float yaw, float pitch, float roll){
    //camera direction and position
    direction = glm::vec3(0,0,0);
    this->position = position;

    //create the mouse x and y
    xPos = 0;
    yPos = 0;

    //set viewing angles
    this ->pitch = pitch;
    this ->yaw = yaw;
    this ->roll = 0;

    //set speed
    speed = 0.25;
    sensitivity = 0.02;

    //create projection variables
    fov = 45.0;
    minDistance = 0.1;
    maxDistance = 100.0;

}

Camera::Camera(glm::vec3 position){
    //camera direction and position
    direction = glm::vec3(0,0,0);
    this->position = position;

    //create the mouse x and y
    xPos = 0;
    yPos = 0;

    //create viewing angles
    pitch = 0;
    yaw = 90;


    //set speed
    speed = 0.25;
    sensitivity = 0.02;

    //create projection variables
    fov = 45.0;
    minDistance = 0.1;
    maxDistance = 100.0;
}

Camera::Camera(){
    //camer direction and position
    direction = glm::vec3(0,0,0);
    position = glm::vec3(0,0,0);

    //mouse
    xPos = 0;
    yPos = 0;

    //viewing angles
    pitch = 45;
    yaw = 135;

    //set speed
    speed = 0.25;
    sensitivity = 0.02;

    //create projection
    fov = 45.0;
    minDistance = 0.1;
    maxDistance = 100.0;
}

Camera::~Camera(){

}

//calculate the view matrix with mouse position
void Camera::calculateViewMatrix(const Window& window) {

    double mouseX, mouseY;
    glfwGetCursorPos(window.getGLFWwindow(), &mouseX, &mouseY);

    // Calculate mouse delta
    double deltaX = mouseX - xPos;
    double deltaY = yPos - mouseY; // invert Y

    xPos = mouseX;
    yPos = mouseY;

    // Apply to pitch and yaw
    yaw   += deltaX * sensitivity;
    pitch += deltaY * sensitivity;

    // Constrain pitch
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    // Convert angles to direction vector
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction = glm::normalize(direction);

    // Movement
    glm::vec3 right = glm::normalize(glm::cross(direction, glm::vec3(0,1,0)));

    if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_W) == GLFW_PRESS)
        position += direction * speed;
    if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_S) == GLFW_PRESS)
        position -= direction * speed;
    if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_A) == GLFW_PRESS)
        position -= right * speed;
    if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_D) == GLFW_PRESS)
        position += right * speed;

    // Build view matrix
    view = glm::lookAt(position, position + direction, glm::vec3(0,1,0));
}

//create project matrix
void Camera::calculateProjectionMatrix(const Window& window) {
    int width, height;
    glfwGetWindowSize(window.getGLFWwindow(), &width, &height);

    projection = glm::perspective(glm::radians(fov),
                                  (float)width / (float)height,
                                  minDistance,
                                  maxDistance);
}


//return the view matrix
glm::mat4 Camera::getViewMatrix(){
    return this->view;
}

//return the projection matrix
glm::mat4 Camera::getProjectionMatrix(){
    return this->projection;
}

//set the fov 
void Camera::setFOV(int fov){
    this->fov = fov;
}
