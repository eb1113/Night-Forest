#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "foliageSystem.h" 

class TileMap {
public:
    TileMap();
    ~TileMap();

    void generateGrid(int width, int depth, float tileSize);
    void setupBuffers();
    void draw() const;
    float getHeightAt(float x, float z) const;

    void generateFoliage(int seed, int numTreeTypes); // number of tree types passed from main

    const std::vector<TreeInstance>& getTrees() const { return trees; }
    const std::vector<ShrubInstance>& getShrubs() const { return shrubs; }

private:
    std::vector<float> vertices;      // x,y,z,u,v
    std::vector<unsigned int> indices;
    std::vector<std::vector<float>> heights;

    GLuint VAO, VBO, EBO;
    int gridWidth, gridDepth;
    float tileSize;

    std::vector<TreeInstance> trees;     
    std::vector<ShrubInstance> shrubs;    
};
