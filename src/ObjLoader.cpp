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
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v") {
            glm::vec3 pos;
            ss >> pos.x >> pos.y >> pos.z;
            positions.push_back(pos);

        } else if (type == "vt") {
            glm::vec2 t;
            ss >> t.x >> t.y;
            t.y = 1.0f - t.y;   // <‑‑ FIX BLENDER UVs
            texCoords.push_back(t);

        } else if (type == "vn") {
            glm::vec3 n;
            ss >> n.x >> n.y >> n.z;
            normals.push_back(n);

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

                if (!texIdx.empty())
                    v.texCoord = texCoords[std::stoi(texIdx) - 1];
                else
                    v.texCoord = glm::vec2(0.0f);

                if (!normIdx.empty())
                    v.normal = normals[std::stoi(normIdx) - 1];
                else
                    v.normal = glm::vec3(0.0f);

                vertices.push_back(v);
                indices.push_back(vertices.size() - 1);
            }
        }
    }

    // If OBJ had no normals, generate them
    if (normals.empty()) {
        for (auto& v : vertices)
            v.normal = glm::vec3(0.0f);

        for (size_t i = 0; i < indices.size(); i += 3) {
            unsigned int i0 = indices[i];
            unsigned int i1 = indices[i + 1];
            unsigned int i2 = indices[i + 2];

            glm::vec3 p0 = vertices[i0].position;
            glm::vec3 p1 = vertices[i1].position;
            glm::vec3 p2 = vertices[i2].position;

            glm::vec3 n = glm::normalize(glm::cross(p1 - p0, p2 - p0));

            vertices[i0].normal += n;
            vertices[i1].normal += n;
            vertices[i2].normal += n;
        }

        for (auto& v : vertices)
            v.normal = glm::normalize(v.normal);
    }

    return !vertices.empty();
}

