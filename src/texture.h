#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

class Texture {

public:
unsigned int id;

Texture(const std::string& path);
void bind(unsigned int unit = 0);

};

#endif