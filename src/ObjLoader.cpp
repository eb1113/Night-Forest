#include "ObjLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>

ObjLoader::ObjLoader(const std::string& p) : path(p) {}

bool ObjLoader::load() {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ: " << path << std::endl;
        return false;
    }

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v") {
            glm::vec3 pos;
            ss >> pos.x >> pos.y >> pos.z;
            positions.push_back(pos);
        } else if (type == "vn") {
            glm::vec3 n;
            ss >> n.x >> n.y >> n.z;
            normals.push_back(n);
        } else if (type == "vt") {
            glm::vec2 t;
            ss >> t.x >> t.y;
            texCoords.push_back(t);
        } else if (type == "f") {
            std::string vertexStr;
            while (ss >> vertexStr) {
                std::stringstream vs(vertexStr);
                std::string posIdx, texIdx, normIdx;
                getline(vs, posIdx, '/');
                getline(vs, texIdx, '/');
                getline(vs, normIdx, '/');

                Vertex v;
                v.position = positions[std::stoi(posIdx) - 1];
                v.texCoord = texIdx.empty() ? glm::vec2(0.0f) : texCoords[std::stoi(texIdx) - 1];
                v.normal   = normIdx.empty() ? glm::vec3(0.0f, 1.0f, 0.0f) : normals[std::stoi(normIdx) - 1];

                vertices.push_back(v);
                indices.push_back(vertices.size() - 1);
            }
        }
    }

    return !vertices.empty();
}
