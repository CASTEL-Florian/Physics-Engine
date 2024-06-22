#ifndef OPENGL_TEST_EBO_H
#define OPENGL_TEST_EBO_H

#include <glad/glad.h>
#include <vector>

class EBO {
public:
    GLuint ID;
    EBO(std::vector<GLuint>& indices);
    void Bind();
    void Unbind();
    void Delete();
};

#endif //OPENGL_TEST_EBO_H