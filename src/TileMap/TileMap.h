#ifndef TILEMAP_H
#define TILEMAP_H
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "PerlinNoise.hpp"

class TileMap {
    public:
        TileMap();
        ~TileMap();

        void generateGrid(int width, int depth, float tileSize);
        void draw();
        void setupBuffers();
        float getHeightAt(float x, float z) const;

    private:
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        unsigned int VAO, VBO, EBO;

        int gridWidth, gridDepth;
        float tileSize;

        std::vector<std::vector<float>> heights;

};
#endif