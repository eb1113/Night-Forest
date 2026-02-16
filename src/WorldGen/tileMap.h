#ifndef TILEMAP_H
#define TILEMAP_H
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "PerlinNoise.hpp"
#include "foliageSystem.h"

class TileMap {
    public:
        TileMap();
        ~TileMap();

        void generateGrid(int width, int depth, float tileSize);
        void draw();
        void setupBuffers();
        float getHeightAt(float x, float z) const;

        // For foliage
        void generateFoliage(int seed);
        const std::vector<TreeInstance>& getTrees() const {return trees;}
        const std::vector<ShrubInstance>& getShrubs() const {return shrubs;}

    private:
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        unsigned int VAO, VBO, EBO;

        int gridWidth, gridDepth;
        float tileSize;

        std::vector<std::vector<float>> heights;
        std::vector<TreeInstance> trees;
        std::vector<ShrubInstance> shrubs;
        bool foliageGenerated = false;

};
#endif