#include "foliageSystem.h"
#include <random>
#include <cmath>
#include <algorithm>

// ------------------------------------------------------------
// Simple hash-based noise
// ------------------------------------------------------------
float FoliageGenerator::simpleNoise(float x, float y, int seed) {
    int n = (int)(x * 37461393 + y * 668265263 + seed * 1442695040888963407);
    n = (n << 13) ^ n;
    return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589)
        & 0x7fffffff) / 1073741824.0f);
}

// ------------------------------------------------------------
// Generate cluster centers
// ------------------------------------------------------------
std::vector<FoliageCluster> FoliageGenerator::generateClusters(
    int gridWidth, int gridDepth, float tileSize, int seed)
{
    std::vector<FoliageCluster> clusters;

    float worldWidth = gridWidth * tileSize;
    float worldDepth = gridDepth * tileSize;

    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    // --------------------------------------------------------
    // Tuned for walkable forest
    // --------------------------------------------------------
    int clusterSpacing = 50;        // fewer clusters
    float clusterThreshold = 0.8f;  // only strong noise creates clusters

    for (int i = 0; i < worldWidth; i += clusterSpacing) {
        for (int j = 0; j < worldDepth; j += clusterSpacing) {

            float noiseValue =
                (simpleNoise(i * 0.01f, j * 0.01f, seed) + 1.0f) * 0.5f;

            if (noiseValue <= clusterThreshold)
                continue;

            FoliageCluster cluster;

            // Random offset inside the grid cell
            cluster.center = glm::vec2(
                i + dist(rng) * clusterSpacing,
                j + dist(rng) * clusterSpacing
            );

            // Smaller clusters for walkable forest
            cluster.radius  = 8.0f + noiseValue * 12.0f;
            cluster.density = noiseValue;

            // Type selection
            float typeNoise =
                (simpleNoise(i * 0.0005f, j * 0.0005f, seed + 1000) + 1.0f) * 0.5f;

            if (typeNoise < 0.4f)
                cluster.vegetationType = 0; // trees
            else if (typeNoise < 0.7f)
                cluster.vegetationType = 1; // shrubs
            else
                cluster.vegetationType = 2; // mixed

            clusters.push_back(cluster);
        }
    }

    return clusters;
}

// ------------------------------------------------------------
// Check spacing between objects
// ------------------------------------------------------------
template <typename T>
bool FoliageGenerator::isValidPosition(glm::vec3 pos,
    const std::vector<T>& existing, float minDistance)
{
    for (const auto& obj : existing) {
        float dx = pos.x - obj.position.x;
        float dz = pos.z - obj.position.z;
        float distSq = dx * dx + dz * dz;

        if (distSq < minDistance * minDistance)
            return false;
    }
    return true;
}

// ------------------------------------------------------------
// Path exclusion helper
// ------------------------------------------------------------
bool FoliageGenerator::isNearPath(glm::vec3 pos) {
    float pathZ = 50.0f;     // example straight path at z = 50
    float pathWidth = 6.0f;  // walkable width

    return fabs(pos.z - pathZ) < pathWidth;
}

// ------------------------------------------------------------
// Generate trees inside a cluster
// ------------------------------------------------------------
std::vector<TreeInstance> FoliageGenerator::generateTrees(
    const FoliageCluster& cluster,
    std::function<float(float, float)> getHeightFunc,
    int seed)
{
    std::vector<TreeInstance> trees;

    std::mt19937 rng(seed + (int)cluster.center.x + (int)cluster.center.y * 1000);
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.14159265f);
    std::uniform_real_distribution<float> distDist(0.0f, 1.0f);
    std::uniform_real_distribution<float> scaleDist(0.8f, 1.2f);
    std::uniform_int_distribution<int> typeDist(0, 2);

    float minDistance = 6.0f;

    // Fewer trees per cluster
    int maxAttempts = (int)(cluster.radius * cluster.radius * cluster.density * 0.2f);
    maxAttempts = std::clamp(maxAttempts, 10, 200);

    for (int attempt = 0; attempt < maxAttempts; ++attempt) {
        float angle = angleDist(rng);
        float distance = sqrt(distDist(rng)) * cluster.radius;

        glm::vec3 position;
        position.x = cluster.center.x + cos(angle) * distance;
        position.z = cluster.center.y + sin(angle) * distance;
        position.y = getHeightFunc(position.x, position.z);

        // Skip trees on the path
        if (isNearPath(position))
            continue;

        if (isValidPosition(position, trees, minDistance)) {
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

// ------------------------------------------------------------
// Generate shrubs (similar logic)
// ------------------------------------------------------------
std::vector<ShrubInstance> FoliageGenerator::generateShrubs(
    const FoliageCluster& cluster,
    std::function<float(float, float)> getHeightFunc,
    int seed)
{
    std::vector<ShrubInstance> shrubs;

    std::mt19937 rng(seed + (int)cluster.center.x + (int)cluster.center.y + 5000);
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.14159265f);
    std::uniform_real_distribution<float> distDist(0.0f, 1.0f);
    std::uniform_real_distribution<float> scaleDist(0.5f, 1.0f);

    float minDistance = 2.0f;

    int maxAttempts = (int)(cluster.radius * cluster.radius * cluster.density);
    maxAttempts = std::clamp(maxAttempts, 20, 400);

    for (int attempt = 0; attempt < maxAttempts; ++attempt) {
        float angle = angleDist(rng);
        float distance = sqrt(distDist(rng)) * cluster.radius;

        glm::vec3 position;
        position.x = cluster.center.x + cos(angle) * distance;
        position.z = cluster.center.y + sin(angle) * distance;
        position.y = getHeightFunc(position.x, position.z);

        if (isNearPath(position))
            continue;

        if (isValidPosition(position, shrubs, minDistance)) {
            ShrubInstance shrub;
            shrub.position = position;
            shrub.scale = scaleDist(rng);
            shrub.rotation = angleDist(rng);
            shrubs.push_back(shrub);
        }
    }

    return shrubs;
}
