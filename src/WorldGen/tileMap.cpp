#include "TileMap.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include "perlinNoise.hpp"

TileMap::TileMap() : VAO(0), VBO(0), EBO(0), gridWidth(0), gridDepth(0), tileSize(1.0f) {}

TileMap::~TileMap() {
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
    heights.clear();
    heights.resize(gridWidth+1, std::vector<float>(gridDepth+1, 0.0f));

    float scale = 0.02f;
    float amplitude = 10.0f;

    for (int x = 0; x < gridWidth; ++x) {
        for (int z = 0; z < gridDepth; ++z) {
            float x0 = x * tileSize;
            float z0 = z * tileSize;
            float x1 = x0 + tileSize;
            float z1 = z0 + tileSize;

            float h00 = PerlinNoise::perlin(x0*scale, z0*scale) * amplitude;
            float h10 = PerlinNoise::perlin(x1*scale, z0*scale) * amplitude;
            float h11 = PerlinNoise::perlin(x1*scale, z1*scale) * amplitude;
            float h01 = PerlinNoise::perlin(x0*scale, z1*scale) * amplitude;

            heights[x][z] = h00;
            heights[x+1][z] = h10;
            heights[x+1][z+1] = h11;
            heights[x][z+1] = h01;

            vertices.insert(vertices.end(), { x0,h00,z0, 0.0f,0.0f });
            vertices.insert(vertices.end(), { x1,h10,z0, 1.0f,0.0f });
            vertices.insert(vertices.end(), { x1,h11,z1, 1.0f,1.0f });
            vertices.insert(vertices.end(), { x0,h01,z1, 0.0f,1.0f });

            unsigned int baseIndex = x*gridDepth*4 + z*4;
            indices.insert(indices.end(), {
                baseIndex, baseIndex+1, baseIndex+2,
                baseIndex+2, baseIndex+3, baseIndex
            });
        }
    }
}

void TileMap::setupBuffers() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void TileMap::draw() const {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

float TileMap::getHeightAt(float x, float z) const {
    float fx = x/tileSize;
    float fz = z/tileSize;

    int x0 = glm::clamp((int)floor(fx),0,gridWidth);
    int z0 = glm::clamp((int)floor(fz),0,gridDepth);
    int x1 = glm::clamp(x0+1,0,gridWidth);
    int z1 = glm::clamp(z0+1,0,gridDepth);

    float sx = fx - x0;
    float sz = fz - z0;

    float h00 = heights[x0][z0];
    float h10 = heights[x1][z0];
    float h01 = heights[x0][z1];
    float h11 = heights[x1][z1];

    float h0 = glm::mix(h00,h10,sx);
    float h1 = glm::mix(h01,h11,sx);
    return glm::mix(h0,h1,sz);
}

void TileMap::generateFoliage(int seed, int numTreeTypes) {
    trees.clear();
    shrubs.clear();
    srand(seed);

    for (int i = 0; i < gridWidth; ++i) {
        for (int j = 0; j < gridDepth; ++j) {
            if ((rand()%100) < 5) { // 5% chance for a tree
                TreeInstance tree;
                tree.position = glm::vec3(i*tileSize, getHeightAt(i*tileSize,j*tileSize), j*tileSize);
                tree.rotation = glm::radians((float)(rand()%360));
                tree.scale = 0.8f + ((rand()%20)/10.0f); // scale 0.8-2.0
                if(numTreeTypes>0) tree.treeType = rand() % numTreeTypes;
                else tree.treeType = 0;
                trees.push_back(tree);
            }
        }
    }
}
