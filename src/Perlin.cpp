#include "Perlin.h"
#include <random>
#include <algorithm>

Perlin::Perlin(int seed) {
    Gradient.resize(IYMAX, std::vector<std::vector<float>>(IXMAX, std::vector<float>(2)));
    generateGradients(seed);
}

void Perlin::generateGradients(int seed) {
    std::mt19937 generator(seed);
    std::uniform_real_distribution<float> distribution(0.0f, 2.0f * M_PI);

    for (int y = 0; y < IYMAX; ++y) {
        for (int x = 0; x < IXMAX; ++x) {
            float angle = distribution(generator);
            Gradient[y][x][0] = cos(angle);
            Gradient[y][x][1] = sin(angle);
        }
    }
}

float Perlin::smoothstep(float w) {
    if (w <= 0.0f) return 0.0f;
    if (w >= 1.0f) return 1.0f;
    return w * w * (3.0f - 2.0f * w);
}

float Perlin::interpolate(float a0, float a1, float w) {
    return a0 + (a1 - a0) * smoothstep(w);
}

float Perlin::dotGridGradient(int ix, int iy, float x, float y) {
    float dx = x - static_cast<float>(ix);
    float dy = y - static_cast<float>(iy);

    return (dx * Gradient[iy][ix][0] + dy * Gradient[iy][ix][1]);
}

float Perlin::noise(float x, float y) {
    int x0 = static_cast<int>(floor(x));
    int x1 = x0 + 1;
    int y0 = static_cast<int>(floor(y));
    int y1 = y0 + 1;

    float sx = x - static_cast<float>(x0);
    float sy = y - static_cast<float>(y0);

    float n0, n1, ix0, ix1, value;
    n0 = dotGridGradient(x0, y0, x, y);
    n1 = dotGridGradient(x1, y0, x, y);
    ix0 = interpolate(n0, n1, sx);

    n0 = dotGridGradient(x0, y1, x, y);
    n1 = dotGridGradient(x1, y1, x, y);
    ix1 = interpolate(n0, n1, sx);

    value = interpolate(ix0, ix1, sy);

    return value;
}

glm::vec3 Perlin::normal(float x, float y, float terrainHeight, float scale, float delta) {
    float height = noise(x * scale, y * scale) * terrainHeight;
    float normalX = -(noise((x + delta) * scale, y * scale) * terrainHeight - height) / delta;
    float normalY = -(noise(x * scale, (y + delta) * scale) * terrainHeight - height) / delta;
    return glm::normalize(glm::vec3(normalX, 1.0f, normalY));
}
