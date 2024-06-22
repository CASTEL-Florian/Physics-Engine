#include "VBO.h"
#ifndef OPENGL_TEST_SPHERE_H
#define OPENGL_TEST_SPHERE_H


class Sphere {
public:
    float x, y, z; // Position
    float radius;

    Sphere(float x, float y, float z, float radius);
    static void generateSphere(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, float radius, unsigned int rings, unsigned int sectors);
};


#endif //OPENGL_TEST_SPHERE_H
