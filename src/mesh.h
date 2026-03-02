#pragma once
#include <vector>
#include "Vertex.h"
#include <glad/glad.h>

struct TreeInstanceData {
    glm::vec3 position;  
    float rotation;     
    float scale;        
};

class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    unsigned int VAO, VBO, EBO;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    void draw();
     void addInstanceBuffer(unsigned int instanceVBO);

private:
    void setupMesh();
};
