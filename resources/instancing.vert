#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;
layout (location = 4) in mat4 instanceMatrix;


out vec3 color;
out vec2 texCoord;
out vec3 currentPos;
out vec3 Normal;

uniform mat4 camMatrix;

void main()
{
   currentPos = vec3(instanceMatrix * vec4(aPos, 1.0));
   gl_Position = camMatrix * vec4(currentPos, 1.0);
   color = aColor;
   texCoord = aTex;
   Normal = aNormal;
}