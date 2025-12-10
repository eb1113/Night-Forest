#include "ObjLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>

bool ObjLoader::load(const std::string& path)
{
    vertices.clear();
    indices.clear();
    std::ifstream file(path);
    if(!file.is_open()){
        std::cerr << "Failed to open OBJ file: " << path << std::endl;
        return false;
    }

    std::vector<glm::vec3> tempVerts;
    std::string line;
    while(std::getline(file, line)){
        std::istringstream ss(line);
        std::string type;
        ss >> type;

        if(type == "v"){
            float x, y, z;
            ss >> x >> y >> z;
            tempVerts.push_back(glm::vec3(x, y, z));
        } else if(type == "f"){
            unsigned int a, b, c;
            ss >> a >> b >> c;
            indices.push_back(a - 1); // OBJ is 1-indexed
            indices.push_back(b - 1);
            indices.push_back(c - 1);
        }
    }

    // Flatten vertices into float array
    for(const auto& v : tempVerts){
        vertices.push_back(v.x);
        vertices.push_back(v.y);
        vertices.push_back(v.z);
    }

    return true;
}
