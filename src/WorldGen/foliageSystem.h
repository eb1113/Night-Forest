#pragma once

#include <vector>
#include <functional>
#include <glm/glm.hpp>



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
    int vegetationType; 
    // 0 trees, 1 shrubs, 2 both
};

class FoliageGenerator {
    public:
        static std::vector<FoliageCluster> generateClusters(int gridWidth, int gridDepth, float tileSize, int seed);
        static std::vector<TreeInstance> generateTrees(const FoliageCluster& cluster, std::function<float(float,float)> getHeightFunc, int seed);
        static std::vector<ShrubInstance> generateShrubs(const FoliageCluster& cluster, std::function<float(float,float)> getHeightFunc, int seed);

    private:
        template <typename T> 
        static bool isValidPosition(glm::vec3 pos, const std::vector<T>& exsisting, float minDistance);
        static float simpleNoise(float x, float y, int seed);
};

