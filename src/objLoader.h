#pragma once
#include <string>
#include <vector>
#include "Vertex.h"

class ObjLoader {
public:
    ObjLoader(const std::string& path);
    bool load();
    const std::vector<Vertex>& getVertices() const { return vertices; }
    const std::vector<unsigned int>& getIndices() const { return indices; }

private:
    std::string path;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};
