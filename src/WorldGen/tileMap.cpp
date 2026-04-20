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

// void TileMap::generateGrid(int width, int depth, float tileSize) {
//     gridWidth = width;
//     gridDepth = depth;
//     this->tileSize = tileSize;

//     vertices.clear();
//     indices.clear();
//     heights.clear();
//     heights.resize(gridWidth + 1, std::vector<float>(gridDepth + 1, 0.0f));

//     float scale = 0.02f;
//     float amplitude = 10.0f;


//     // Generate heightmap + raw vertices
//     for (int x = 0; x < gridWidth; ++x) {
//         for (int z = 0; z < gridDepth; ++z) {

//             float x0 = x * tileSize;
//             float z0 = z * tileSize;
//             float x1 = x0 + tileSize;
//             float z1 = z0 + tileSize;

//             float h00 = PerlinNoise::perlin(x0 * scale, z0 * scale) * amplitude;
//             float h10 = PerlinNoise::perlin(x1 * scale, z0 * scale) * amplitude;
//             float h11 = PerlinNoise::perlin(x1 * scale, z1 * scale) * amplitude;
//             float h01 = PerlinNoise::perlin(x0 * scale, z1 * scale) * amplitude;

//             heights[x][z]     = h00;
//             heights[x+1][z]   = h10;
//             heights[x+1][z+1] = h11;
//             heights[x][z+1]   = h01;

//             vertices.insert(vertices.end(), { x0, h00, z0,  0,0,0,  0.0f, 0.0f });
//             vertices.insert(vertices.end(), { x1, h10, z0,  0,0,0,  1.0f, 0.0f });
//             vertices.insert(vertices.end(), { x1, h11, z1,  0,0,0,  1.0f, 1.0f });
//             vertices.insert(vertices.end(), { x0, h01, z1,  0,0,0,  0.0f, 1.0f });

//             unsigned int baseIndex = (x * gridDepth + z) * 4;

//             indices.insert(indices.end(), {
//                 baseIndex, baseIndex + 1, baseIndex + 2,
//                 baseIndex + 2, baseIndex + 3, baseIndex
//             });
//         }
//     }


//     // Compute normals
//     for (int i = 0; i < indices.size(); i += 3) {
//         unsigned int i0 = indices[i] * 8;
//         unsigned int i1 = indices[i+1] * 8;
//         unsigned int i2 = indices[i+2] * 8;

//         glm::vec3 v0(vertices[i0],     vertices[i0+1],     vertices[i0+2]);
//         glm::vec3 v1(vertices[i1],     vertices[i1+1],     vertices[i1+2]);
//         glm::vec3 v2(vertices[i2],     vertices[i2+1],     vertices[i2+2]);

//         glm::vec3 e1 = v1 - v0;
//         glm::vec3 e2 = v2 - v0;
//         glm::vec3 normal = glm::normalize(glm::cross(e1, e2));

//         // Accumulate
//         vertices[i0+3] += normal.x;
//         vertices[i0+4] += normal.y;
//         vertices[i0+5] += normal.z;

//         vertices[i1+3] += normal.x;
//         vertices[i1+4] += normal.y;
//         vertices[i1+5] += normal.z;

//         vertices[i2+3] += normal.x;
//         vertices[i2+4] += normal.y;
//         vertices[i2+5] += normal.z;
//     }


//     // Normalize all normals
//     for (int i = 0; i < vertices.size(); i += 8) {
//         glm::vec3 n(vertices[i+3], vertices[i+4], vertices[i+5]);
//         n = glm::normalize(n);

//         vertices[i+3] = n.x;
//         vertices[i+4] = n.y;
//         vertices[i+5] = n.z;
//     }
// }


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
            if ((rand()%100) < 2) { //tuning for the trees
                TreeInstance tree;
                tree.position = glm::vec3(i*tileSize, getHeightAt(i*tileSize,j*tileSize), j*tileSize);
                tree.rotation = glm::radians((float)(rand()%360));
                tree.scale = 0.8f + ((rand()%20)/10.0f); 
                if(numTreeTypes>0) tree.treeType = rand() % numTreeTypes;
                else tree.treeType = 0;
                trees.push_back(tree);
            }
        }
    }

    // generating shrubs hope that this logic work  :)
    //update it did: the for loop with the random helps with density
    std::vector<FoliageCluster> shrubClusters;
    shrubClusters.reserve(trees.size() / 2 + 16);

    for (const TreeInstance& tree : trees) {
        if ((rand() % 100) >= 15) { //tunning
            continue;
        }

        FoliageCluster cluster;
        cluster.center = glm::vec2(
            tree.position.x + ((rand() % 100) / 100.0f - 0.5f) * 6.0f,
            tree.position.z + ((rand() % 100) / 100.0f - 0.5f) * 6.0f);
        cluster.radius = 3.5f + ((rand() % 100) / 100.0f) * 3.0f;
        cluster.density = 0.45f + ((rand() % 100) / 100.0f) * 0.4f;
        cluster.vegetationType = 1;
        shrubClusters.push_back(cluster);
    }


    for (int i = 0; i < gridWidth; i += 18) {
        for (int j = 0; j < gridDepth; j += 18) {
            if ((rand() % 100) >= 4) {
                continue;
            }

            FoliageCluster cluster;
            cluster.center = glm::vec2(
                i * tileSize + ((rand() % 100) / 100.0f) * 18.0f * tileSize,
                j * tileSize + ((rand() % 100) / 100.0f) * 18.0f * tileSize);
            cluster.radius = 4.0f + ((rand() % 100) / 100.0f) * 4.0f;
            cluster.density = 0.35f + ((rand() % 100) / 100.0f) * 0.3f;
            cluster.vegetationType = 1;
            shrubClusters.push_back(cluster);
        }
    }

    printf("Generated %zu shrub clusters\n", shrubClusters.size());

    for (size_t clusterIndex = 0; clusterIndex < shrubClusters.size(); ++clusterIndex) {
        std::vector<ShrubInstance> clusterShrubs = FoliageGenerator::generateShrubs(
            shrubClusters[clusterIndex],
            [this](float x, float z) { return getHeightAt(x, z); },
            seed + static_cast<int>(clusterIndex) * 17);

        shrubs.insert(shrubs.end(), clusterShrubs.begin(), clusterShrubs.end());
    }

}
