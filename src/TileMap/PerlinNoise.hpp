#ifndef PERLIN_NOISE_HPP
#define PERLIN_NOISE_HPP

#include <glm/glm.hpp>

namespace PerlinNoise {

    // Sample 2D Perlin noise at coordinates (x, y)
    float perlin(float x, float y);

} 

#endif // PERLIN_NOISE_HPP
