#pragma once
#include <vector>

struct TreeInstance {
    glm::vec3 position;
    float scale;
    float rotation;
    int treeType;

};

struct ShrubInstance {
    glm::vec3 position;
    float scale;
    float rotation;
    //int shrubType;

};

struct FoliageCluster {
    glm::vec2 center;
    float radius;
    float density;
    int vegetationType; // 0 for tree, 1 for shrubk, 3 for mixed
};

class FoliageGenerator {
    public:
        static std::vector<FoliageCluster> generateClusters(int gridWidth, int gridDepth, float tileSize, int seed);
        static std::vector<TreeInstance> generateTrees(const FoliageCluster& cluster, float(*getHeightFunc)(float,float), int seed);
        static std::vector<ShrubInstance> generateShrubs(const FoliageCluster& cluster, float(*getHeightFunc)(float,float), int seed);

    private:
        static bool isValidPosition(glm::vec3 pos, const std::vector<TreeInstance>& existing, float minDistance);
        static bool isValidPosition(glm::vec3 pos, const std::vector<ShrubInstance>& existing, float minDistance);
        static float simpleNoise(float x, float y, int seed);
}

