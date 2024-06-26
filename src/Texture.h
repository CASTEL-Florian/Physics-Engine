#ifndef OPENGL_TEST_TEXTURE_H
#define OPENGL_TEST_TEXTURE_H

#include <glad/glad.h>
#include <stb_image.h>

#include "shaderClass.h"

class Texture {
public:
    GLuint ID;
    const char* type;
    GLuint unit;
    Texture(const char* image, const char* texType, GLuint slot, GLenum format, GLenum pixelType);

    void texUnit(Shader &shader, const char* uniform, GLuint unit);
    void Bind();
    void Unbind();
    void Delete();
};

#endif //OPENGL_TEST_TEXTURE_H
