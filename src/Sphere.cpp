#include "Sphere.h"
Sphere::Sphere(float x, float y, float z, float radius): x(x), y(y), z(z), radius(radius) {}

void Sphere::generateSphere(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, float radius, unsigned int rings, unsigned int sectors) {
    const float PI = 3.14159265359f;
    float x, y, z, xy;                              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
    float s, t;                                     // vertex texCoord

    float sectorStep = 2 * PI / sectors;
    float stackStep = PI / rings;
    float sectorAngle, stackAngle;

    for (unsigned int i = 0; i <= rings; ++i) {
        stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        for (unsigned int j = 0; j <= sectors; ++j) {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            vertices.push_back(Vertex{ glm::vec3(x, y, z), glm::vec3(x * lengthInv, y * lengthInv, z * lengthInv), glm::vec3(1.0, 1.0, 1.0), glm::vec2((float)j / sectors, (float)i / rings) });

            // add indices
            if (i != 0 && j != 0) {
                int curRow = i * (sectors + 1);
                int prevRow = (i - 1) * (sectors + 1);

                indices.push_back(prevRow + j - 1);
                indices.push_back(curRow + j);
                indices.push_back(curRow + j - 1);

                indices.push_back(prevRow + j - 1);
                indices.push_back(prevRow + j);
                indices.push_back(curRow + j);
            }
        }
    }
}