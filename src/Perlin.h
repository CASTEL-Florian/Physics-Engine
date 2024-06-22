#ifndef PERLIN_H
#define PERLIN_H

#include <vector>
#include <cmath>
#include <glm/glm.hpp>

class Perlin {
public:
    Perlin(int seed);
    float noise(float x, float y);
    glm::vec3 normal(float x, float y, float terrainHeight, float scale, float delta = 0.1f);

private:
    static const int IYMAX = 256;
    static const int IXMAX = 256;
    std::vector<std::vector<std::vector<float>>> Gradient;

    float smoothstep(float w);
    float interpolate(float a0, float a1, float w);
    float dotGridGradient(int ix, int iy, float x, float y);
    void generateGradients(int seed);
};

#endif // PERLIN_H
