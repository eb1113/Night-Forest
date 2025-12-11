#include "PerlinNoise.hpp"
#include <cmath>

namespace PerlinNoise {

    // Helper functions (internal, not exposed in header)
    namespace {
        float dotGridGradient(int ix, int iy, float x, float y);
        glm::vec2 randomGradient(int ix, int iy);
        float interpolate(float a0, float a1, float w);

    

        float dotGridGradient(int ix, int iy, float x, float y) {
            glm::vec2 gradient = randomGradient(ix, iy);
            float dx = x - float(ix);
            float dy = y - float(iy);
            return (dx * gradient.x + dy * gradient.y);
        }

        glm::vec2 randomGradient(int ix, int iy) {
            const unsigned w = 8 * sizeof(unsigned);
            const unsigned s = w / 2;
            unsigned a = ix, b = iy;
            a *= 3284157443;
            b ^= a << s | a >> (w - s);
            b *= 1911520717;
            float random = a * (3.14159265f / ~(~0u >> 1));

            glm::vec2 v;
            v.x = cos(random);
            v.y = sin(random);
            return v;
        }

        float interpolate(float a0, float a1, float w) {
            return (a1 - a0) * (3.0f - 2.0f * w) * w * w + a0;
        }
    } // anonymous namespace

    
    float perlin(float x, float y) {
        int x0 = int(x);
        int x1 = x0 + 1;
        int y0 = int(y);
        int y1 = y0 + 1;

        float sx = x - float(x0);
        float sy = y - float(y0);

        float n0 = dotGridGradient(x0, y0, x, y);
        float n1 = dotGridGradient(x1, y0, x, y);
        float ix0 = interpolate(n0, n1, sx);

        n0 = dotGridGradient(x0, y1, x, y);
        n1 = dotGridGradient(x1, y1, x, y);
        float ix1 = interpolate(n0, n1, sx);

        return interpolate(ix0, ix1, sy);
    }

} // namespace PerlinNoise
