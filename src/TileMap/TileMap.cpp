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

void TileMap::generateGrid(int width, int depth, float tileSize) {
    gridWidth = width;
    gridDepth = depth;
    this->tileSize = tileSize;

    vertices.clear();
    indices.clear();

    float scale = 0.05f;      // controls frequency of terrain
    float amplitude = 5.0f;  // max height of terrain

    for (int x = 0; x < gridWidth; x++) {
        for (int z = 0; z < gridDepth; z++) {
            // Compute world coordinates
            float x0 = x * tileSize;
            float z0 = z * tileSize;
            float x1 = x0 + tileSize;
            float z1 = z0 + tileSize;

            // Sample Perlin noise for each corner
            float h00 = PerlinNoise::perlin(x0 * scale, z0 * scale) * amplitude;
            float h10 = PerlinNoise::perlin(x1 * scale, z0 * scale) * amplitude;
            float h11 = PerlinNoise::perlin(x1 * scale, z1 * scale) * amplitude;
            float h01 = PerlinNoise::perlin(x0 * scale, z1 * scale) * amplitude;

            // Add vertices (x, y, z, u, v)
            vertices.insert(vertices.end(), { x0, h00, z0, 0.0f, 0.0f });
            vertices.insert(vertices.end(), { x1, h10, z0, 1.0f, 0.0f });
            vertices.insert(vertices.end(), { x1, h11, z1, 1.0f, 1.0f });
            vertices.insert(vertices.end(), { x0, h01, z1, 0.0f, 1.0f });

            // Compute indices for two triangles
            unsigned int baseIndex = x * gridDepth * 4 + z * 4;
            indices.insert(indices.end(), {
                baseIndex, baseIndex + 1, baseIndex + 2,
                baseIndex + 2, baseIndex + 3, baseIndex
            });
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