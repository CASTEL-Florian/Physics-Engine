#ifndef OPENGL_TEST_MESH_H
#define OPENGL_TEST_MESH_H

#include <string>
#include "VAO.h"
#include "EBO.h"
#include "Camera.h"
#include "Texture.h"

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;

    VAO VAO;

    unsigned int instancing;

    Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures);

    void Draw(Shader& shader,
              Camera& camera,
              glm::mat4 matrix,
              GLenum mode = GL_TRIANGLES, unsigned int instancing = 1, std::vector <glm::mat4> instanceMatrix = {});
};

#endif //OPENGL_TEST_MESH_H
