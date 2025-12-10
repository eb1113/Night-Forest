cmake_minimum_required(VERSION 3.10)
project(nightforest)
 
set(CMAKE_CXX_STANDARD 17)

# GLFW
add_subdirectory(external/glfw)

# GLAD
add_library(glad external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

# Your executable
add_executable(nightforest
    src/main.cpp
    src/ObjLoader.cpp
    src/Window.cpp
    src/camera/Camera.cpp
    src/camera/EulerAngle.cpp
)

# Includes
target_include_directories(nightforest PUBLIC
    external/glad/include
    external/glfw/include
    external/glm
    external/stb
)

# Linking
target_link_libraries(nightforest
    glad
    glfw
)