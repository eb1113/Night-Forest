#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb_image.h"
#include "Texture.h"
#include <glad/glad.h>
#include <iostream>
#include <cmath>
#include <vector>

// namespace {
// bool isFireflySpritePath(const std::string& path)
// {
//     return path.find("firefly_sprite") != std::string::npos;
// }

// void uploadProceduralFireflySprite()
// {
//     constexpr int width = 64;
//     constexpr int height = 64;
//     std::vector<unsigned char> pixels(width * height * 4, 0);

//     for (int y = 0; y < height; ++y) {
//         for (int x = 0; x < width; ++x) {
//             const float u = (static_cast<float>(x) + 0.5f) / static_cast<float>(width);
//             const float v = (static_cast<float>(y) + 0.5f) / static_cast<float>(height);
//             const float dx = u - 0.5f;
//             const float dy = v - 0.5f;
//             const float radialDistance = std::sqrt(dx * dx + dy * dy);
//             const float radialGlow = std::max(0.0f, 1.0f - radialDistance / 0.55f);

//             const float abdomenDx = u - 0.5f;
//             const float abdomenDy = v - 0.66f;
//             const float abdomenDistance = std::sqrt(abdomenDx * abdomenDx + abdomenDy * abdomenDy);
//             const float abdomenGlow = std::max(0.0f, 1.0f - abdomenDistance / 0.18f);

//             const float wingLeft = std::max(0.0f, 1.0f - std::sqrt((u - 0.38f) * (u - 0.38f) + (v - 0.46f) * (v - 0.46f)) / 0.16f);
//             const float wingRight = std::max(0.0f, 1.0f - std::sqrt((u - 0.62f) * (u - 0.62f) + (v - 0.46f) * (v - 0.46f)) / 0.16f);
//             const float body = std::max(0.0f, 1.0f - std::sqrt((u - 0.5f) * (u - 0.5f) + (v - 0.54f) * (v - 0.54f)) / 0.12f);

//             const float alpha = std::min(1.0f, abdomenGlow * 1.2f + radialGlow * 0.35f + wingLeft * 0.18f + wingRight * 0.18f + body * 0.35f);

//             const float r = std::min(1.0f, 0.22f * body + 0.18f * radialGlow + 1.0f * abdomenGlow);
//             const float g = std::min(1.0f, 0.16f * body + 0.14f * radialGlow + 0.9f * abdomenGlow);
//             const float b = std::min(1.0f, 0.06f * body + 0.04f * radialGlow + 0.45f * abdomenGlow);

//             const int pixelIndex = (y * width + x) * 4;
//             pixels[pixelIndex + 0] = static_cast<unsigned char>(r * 255.0f);
//             pixels[pixelIndex + 1] = static_cast<unsigned char>(g * 255.0f);
//             pixels[pixelIndex + 2] = static_cast<unsigned char>(b * 255.0f);
//             pixels[pixelIndex + 3] = static_cast<unsigned char>(alpha * 255.0f);
//         }
//     }

//     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
//     glGenerateMipmap(GL_TEXTURE_2D);
// }
// }

Texture::Texture(const std::string& path)
{
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int w, h, channels;
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &channels, 4);
    if (!data) {
        // if (isFireflySpritePath(path)) {
        //     std::cout << "Falling back to procedural firefly texture for: " << path << "\n";
        //     uploadProceduralFireflySprite();
        //     return;
        // }

        std::cout << "Failed to load texture: " << path << "\n";
        return;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}

void Texture::bind(unsigned int unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id);
}
