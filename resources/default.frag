#version 330 core
out vec4 FragColor;

in vec3 color;
in vec2 texCoord;

in vec3 Normal;
in vec3 currentPos;

uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 cameraPos;

vec4 pointLight(){
    vec3 lightVec = lightPos - currentPos;
    float distance = length(lightVec);
    float a = 1.0;
    float b = 0.4;
    float intensity = 1.0 / (a * distance * distance + b * distance + 1.0);

    float ambientStrength = 0.2;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightVec);
    float diff = max(dot(norm, lightDir), 0.0);

    float specularStrength = 0.5;
    vec3 viewDir = normalize(cameraPos - currentPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8);
    float specular = specularStrength * spec;
    return (texture(diffuse0, texCoord) * vec4(color, 1.0) * (diff * intensity + ambientStrength) + texture(specular0, texCoord).r * specular * intensity) * lightColor;
}

vec4 directionalLight(){
    vec3 lightDir = normalize(lightPos);
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    float ambientStrength = 0.2;

    float specularStrength = 0.5;
    vec3 viewDir = normalize(cameraPos - currentPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8);
    float specular = specularStrength * spec;

    return (texture(diffuse0, texCoord) * vec4(color, 1.0) * (diff + ambientStrength) + texture(specular0, texCoord).r * specular) * lightColor;
}

vec4 spotLight(){
    float outerCone = 0.9f;
    float innerCone = 0.95f;

    vec3 lightVec = lightPos - currentPos;

    float ambientStrength = 0.2;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightVec);
    float diff = max(dot(norm, lightDir), 0.0f);

    float specularStrength = 0.5;
    vec3 viewDir = normalize(cameraPos - currentPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8);
    float specular = specularStrength * spec;

    float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDir);
    float intensity = clamp((angle - outerCone) / (innerCone - outerCone), 0.0, 1.0);

    return (texture(diffuse0, texCoord) * vec4(color, 1.0) * (diff * intensity + ambientStrength) + texture(specular0, texCoord).r * specular * intensity) * lightColor;
}

void main()
{
    FragColor = directionalLight();
}