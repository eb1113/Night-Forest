#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <vector>
#include <string>
#include <glm/glm.hpp>

class ObjLoader {
private:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

public:
    ObjLoader() = default;

    bool load(const std::string& path);

    const std::vector<float>& getVertices() const { return vertices; }
    const std::vector<unsigned int>& getIndices() const { return indices; }
};

#endif
