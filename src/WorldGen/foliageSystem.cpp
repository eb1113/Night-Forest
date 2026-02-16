#include "foliageSystem.h"
#include <random>
 #include <cmath>

 //hash based noise for clusters
 float FoliageGenerator::simpleNoise(float x, float y, int seed){
    int n = (int)(x * 37461393 + y * 668265263 + seed * 1442695040888963407);
    n = (n << 13) ^ n;
    return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
 }

 std::vector<FoliageCluster> FoliageGenerator::generateClusters(int gridWidth, int gridDepth, float tileSize, int seed) {
    std::vector<FoliageCluster> clusters;
    float worldWidth = gridWidth * tileSize;
    float worldDepth = gridDepth * tileSize;

    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    //cluster centers with low frequency noise
    int clusterSpacing = 30;
    float clusterThreshold = 0.3f; //subject to change

    for(int i = 0; i < worldWidth; i += clusterSpacing){
        for(int j = 0; j < worldDepth; j += clusterSpacing){
            float noiseValue = (simpleNoise(i * 0.01f, j * 0.01f, seed)+ 1.0f) * 0.5f;

            if(noiseValue > clusterThreshold){
                FoliageCluster cluster;
                cluster.center = glm::vec2(i + dist(rng) * clusterSpacing, j + dist(rng) * clusterSpacing);
            };
            cluster.radius = 15.0f + noiseValue * 25.0f;
            cluster.density = noiseValue;

            //deciding type of foliage
            float typeNoise = (simpleNoise(i * 0.0005f, j * 0.000f, seed + 1000) + 1.0f) * 0.5f;
            if(typeNoise < 0.4f){
                cluster.vegetationType = 0; //trees
            } else if(typeNoise < 0.7f){
                cluster.vegetationType = 1; //shrubs
            } else {
                cluster.vegetationType = 2; //mixed
            }
            clusters.push_back(cluster);
        }
    }
    return clusters;
   
}

bool FoliageGenerator::isValidPosition(glm::vec3 pos, const std::vector<TreeInstance>& exsisting, float minDistance) {
    for(const auto& tree : exsisting){
        float dx = pos.x - tree.position.x;
        float dz = pos.z - tree.position.z;
        float distSq = dx * dx + dz * dz;
        if(distSq < minDistance * minDistance){
            return false;
        }
    
    }
    return true;
}

std::vector<TreeInstance> FoliageGenerator::generateTrees(const FoliageCluster& cluster, float (*getHeightFunc)(float, float), int seed){
    std::vector<TreeInstance> trees;
    std::mt19937 rng(seed + (int)cluster.center.x + (int)cluster.center.y * 1000);
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.14159265f);
    std::uniform_real_distribution<float> distDist(0.0f, 1.0f);
    std::uniform_real_distribution<float> scaleDist(0.8f, 1.2f);
    std::uniform_int_distribution<int> typeDist(0, 2); 

    float minDistance = 4.0f;
    int maxAttempts = (int)(cluster.radius * cluster.radius * cluster.density * 0.5f);
    maxAttempts = glm::clamp(maxAttempts, 10, 200);


    //simplified poisson disk sampling
    for(int attempt = 0; attempt < maxAttempts; ++attempt){
        float angle = angleDist(rng);
        float distance = sqrt(distDist(rng)) * cluster.radius;

        glm::vec3 position;
        position.x = cluster.center.x + cos(angle) * distance;
        position.z = cluster.center.y + sin(angle) * distance;
        position.y = getHeightFunc(position.x, position.z);

        if(isValidPosition(position, trees, minDistance)){
            TreeInstance tree;
            tree.position = position;
            tree.scale = scaleDist(rng);
            tree.rotation = angleDist(rng);
            tree.treeType = typeDist(rng);
            trees.push_back(tree);
        }
    }
    return trees;
}

std::vector<ShrubInstance> FoliageGenerator::generateShrubs(const FoliageCluster& cluster, float (*getHeightFunc)(float, float), int seed){
    std::vector<ShrubInstance> shrubs;
    std::mt19937 rng(seed + (int)cluster.center.x + (int)cluster.center.y + 5000);
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.14159265f);
    std::uniform_real_distribution<float> distDist(0.0f, 1.0f);
    std::uniform_real_distribution<float> scaleDist(0.5f, 1.0f);

    float minDistance = 2.0f;
    int maxAttempts = (int)(cluster.radius * cluster.radius * cluster.density);
    maxAttempts = glm::clamp(maxAttempts, 20, 400);

    for(int attempt = 0; attempt < maxAttempts; ++attempt){
        float angle = angleDist(rng);
        float distance = sqrt(distDist(rng)) * cluster.radius;

        glm::vec3 position;
        position.x = cluster.center.x + cos(angle) * distance;
        position.z = cluster.center.y + sin(angle) * distance;
        position.y = getHeightFunc(position.x, position.z);

        if(isValidPosition(position, shrubs, minDistance)){
            ShrubInstance shrub;
            shrub.position = position;
            shrub.scale = scaleDist(rng);
            shrub.rotation = angleDist(rng);
            shrubs.push_back(shrub);
        }
    }
    return shrubs;
}



    