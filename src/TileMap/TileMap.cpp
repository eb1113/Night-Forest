#include "TileMap.h"

TileMap::TileMap() {
    //initalize VAO, VBO, EBO
    VAO = 0;
    VBO = 0;
    EBO = 0;


}

TileMap::~TileMap() {
    // Clean up VAO, VBO, EBO
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void TileMap::generateGrid(int width, int depth, float tileSize){
    gridWidth = width;
    gridDepth = depth;
    this->tileSize = tileSize;
    //generate vertices and indices procedurally
    for(int x = 0; x < gridWidth; x++){
        for(int y = 0; y < gridDepth; y++){
            float x1 = x * tileSize;
            float z1 = y * tileSize;
            float x2 = x1 + tileSize;
            float z2 = z1 + tileSize;

            // Add vertices
            vertices.push_back(x1);
            vertices.push_back(0.0f);
            vertices.push_back(z1);
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);

            vertices.push_back(x2);
            vertices.push_back(0.0f);
            vertices.push_back(z1);
            vertices.push_back(1.0f);
            vertices.push_back(0.0f);

            vertices.push_back(x2);
            vertices.push_back(0.0f);
            vertices.push_back(z2);
            vertices.push_back(1.0f);
            vertices.push_back(1.0f);

            vertices.push_back(x1);
            vertices.push_back(0.0f);
            vertices.push_back(z2);
            vertices.push_back(0.0f);
            vertices.push_back(1.0f);

            // Add indices
            unsigned int baseIndex = x * gridDepth * 4 + y * 4;
            indices.push_back(baseIndex);
            indices.push_back(baseIndex + 1);
            indices.push_back(baseIndex + 2);
            indices.push_back(baseIndex + 2);
            indices.push_back(baseIndex + 3);
            indices.push_back(baseIndex);
        }
    }

}
void TileMap::setupBuffers() {
    // Set up VAO, VBO, EBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Set up vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}
void TileMap::draw(){
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}