#include "Mesh.h"
#include "stb_image.h"
#include "Perlin.h"
#include "Rigidbody.h"
#include "SpatialGrid.h"
#include "Collision.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <thread>
#include <future>

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
const unsigned int TERRAIN_SIZE = 100;
const unsigned int TERRAIN_HEIGHT = 5;
const float NORMAL_DELTA = 0.1;
const float SCALE = 0.1f;

Vertex lightVertices[] = {
        Vertex{glm::vec3(-0.1f,-0.1f,0.1f)},
        Vertex{glm::vec3(-0.1f,-0.1f,-0.1f)},
        Vertex{glm::vec3(0.1f,-0.1f,-0.1f)},
        Vertex{glm::vec3(0.1f,-0.1f,0.1f)},
        Vertex{glm::vec3(-0.1f,0.1f,0.1f)},
        Vertex{glm::vec3(-0.1f,0.1f,-0.1f)},
        Vertex{glm::vec3(0.1f,0.1f,-0.1f)},
        Vertex{glm::vec3(0.1f,0.1f,0.1f)},
};

GLuint lightIndices[] = {
        0, 1, 2,
        0, 2, 3,
        0,4,7,
        0,7,3,
        3,7,6,
        3,6,2,
        2,6,5,
        2,5,1,
        1,5,4,
        1,4,0,
        4,5,6,
        4,6,7
};



std::vector<Vertex> generateTerrain(Perlin& perlin) {
    std::vector<Vertex> vertices;
    for (unsigned int y = 0; y < TERRAIN_SIZE; ++y) {
        for (unsigned int x = 0; x < TERRAIN_SIZE; ++x) {
            float height = perlin.noise(x * SCALE, y * SCALE) * TERRAIN_HEIGHT;

            glm::vec3 normal = perlin.normal(x, y, TERRAIN_HEIGHT, SCALE, NORMAL_DELTA) ;
            vertices.push_back(Vertex{
                    glm::vec3(x, height, y),   // Position
                    normal, // Normal
                    glm::vec3(1.0f, 1.0f, 1.0f), // Color (placeholder)
                    glm::vec2(x, y) // TexCoords
            });
        }
    }
    return vertices;
}


std::vector<GLuint> generateTerrainIndices() {
    std::vector<GLuint> indices;
    for (unsigned int y = 0; y < TERRAIN_SIZE - 1; ++y) {
        for (unsigned int x = 0; x < TERRAIN_SIZE; ++x) {
            indices.push_back(y * TERRAIN_SIZE + x);
            indices.push_back((y + 1) * TERRAIN_SIZE + x);
        }
        if (y < TERRAIN_SIZE - 2) {
            indices.push_back((y + 1) * TERRAIN_SIZE + (TERRAIN_SIZE - 1));
            indices.push_back((y + 1) * TERRAIN_SIZE);
        }
    }
    return indices;
}

void findCollisions(std::vector<Rigidbody>& vec, SpatialGrid& grid, size_t start, size_t end, std::promise<std::vector<Collision>> &&result){
    // Check for collision with other rigidbodies
    std::vector<Collision> collisions;
    for (size_t i = start; i < end; ++i) {
        Rigidbody& rb = vec[i];
        std::vector<Rigidbody *> potentialCollisions = grid.getPotentialCollisions(&rb);
        for (Rigidbody *other: potentialCollisions) {
            if (&rb == other || rb.id > other -> id) continue;
            glm::vec3 direction = other->position - rb.position;
            float distance = glm::length(direction);
            float sumRadii = rb.sphere.radius + other->sphere.radius;
            if (distance < sumRadii) {
                glm::vec3 normal = glm::normalize(direction);

                glm::vec3 relativeVelocity = rb.velocity - other->velocity;
                float dotProduct = glm::dot(relativeVelocity, normal);
                if (dotProduct > 0.0f) {
                    float e = 0.9;
                    float j = -(1.0f + e) * dotProduct / (1.0f / rb.mass + 1.0f / other->mass);
                    glm::vec3 impulse = j * normal;
                    collisions.emplace_back(&rb, other, impulse);
                }
            }
        }
    }
    result.set_value(collisions);
}


int main() {
    std::cout << "Hello, World!" << std::endl;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    Texture textures[]{
            Texture("resources/Stylized_Grass_002_basecolor.jpg", "diffuse", 0, GL_RGB, GL_UNSIGNED_BYTE),
            Texture("resources/Stylized_Grass_002_roughness.jpg", "specular", 1, GL_RGB, GL_UNSIGNED_BYTE)
    };

    Texture blankTextures[]{
            Texture("resources/spec.jpg", "diffuse", 0, GL_RGB, GL_UNSIGNED_BYTE),
            Texture("resources/spec.jpg", "specular", 1, GL_RGB, GL_UNSIGNED_BYTE)
    };

    Shader shaderProgram("resources/default.vert", "resources/default.frag");
    Shader instancingShaderProgram("resources/instancing.vert", "resources/default.frag");

    Perlin perlin(1234); // Initialize Perlin noise generator with a seed
    std::vector<Vertex> terrainVertices = generateTerrain(perlin);
    std::vector<GLuint> terrainIndices = generateTerrainIndices();

    std::vector<Texture> tex(textures, textures + sizeof(textures)/sizeof(Texture));
    std::vector<Texture> blankTex(blankTextures, blankTextures + sizeof(blankTextures)/sizeof(Texture));

    Mesh terrain(terrainVertices, terrainIndices, tex);

    Shader lightShader("resources/light.vert", "resources/light.frag");
    std::vector<Vertex> lightVerts(lightVertices, lightVertices + sizeof(lightVertices)/sizeof(Vertex));
    std::vector<GLuint> lightInd(lightIndices, lightIndices + sizeof(lightIndices)/sizeof(GLuint));
    Mesh light(lightVerts, lightInd, tex);

    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos = glm::vec3(0.0f, 0.5f, 0.0f);
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightPos);
    lightModel = glm::scale(lightModel, glm::vec3(1.f));

    glm::vec3 objectPos = glm::vec3(- ((int)TERRAIN_SIZE / 2), 0.0f, - ((int)TERRAIN_SIZE / 2));
    glm::mat4 objectModel = glm::mat4(1.0f);
    objectModel = glm::translate(objectModel, objectPos);

    std::vector<Vertex> sphereVertices;
    std::vector<GLuint> sphereIndices;
    Sphere::generateSphere(sphereVertices, sphereIndices, 1.0f, 20, 20);
    Mesh sphere(sphereVertices, sphereIndices, blankTex);

    SpatialGrid grid(5.0f);

    std::vector<Rigidbody> rigidbodies;
    // Add 100 random rigidbodies to the grid with diameter between 1.0 and 2.0
    for (int i = 0; i < 1000; ++i) {
        float x = static_cast<float>(rand() % TERRAIN_SIZE) - (TERRAIN_SIZE / 2);
        float y = static_cast<float>(rand() % 10) + TERRAIN_HEIGHT;
        float z = static_cast<float>(rand() % TERRAIN_SIZE) - (TERRAIN_SIZE / 2);
        float radius = static_cast<float>(rand() % 10) / 10.0f + 1.0f;
        Rigidbody rb(glm::vec3(x, y, z), radius, radius * radius * radius);
        rigidbodies.push_back(rb);
    }


    for (Rigidbody& rb : rigidbodies) {
        grid.addSphere(&rb);
    }


    lightShader.Activate();
    glUniform4fv(glGetUniformLocation(lightShader.ID, "lightColor"), 1, glm::value_ptr(lightColor));

    shaderProgram.Activate();
    glUniform4fv(glGetUniformLocation(shaderProgram.ID, "lightColor"), 1, glm::value_ptr(lightColor));
    glUniform3fv(glGetUniformLocation(shaderProgram.ID, "lightPos"), 1, glm::value_ptr(lightPos));

    instancingShaderProgram.Activate();
    glUniform4fv(glGetUniformLocation(instancingShaderProgram.ID, "lightColor"), 1, glm::value_ptr(lightColor));
    glUniform3fv(glGetUniformLocation(instancingShaderProgram.ID, "lightPos"), 1, glm::value_ptr(lightPos));



    glEnable(GL_DEPTH_TEST);

    Camera camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0f, 50.0f, 70.0f));

    while (!glfwWindowShouldClose(window)) {
        while (glfwGetTime() < 1.0 / 60.0) {}
        std::cout << "FPS: " << 1.0 / glfwGetTime() << std::endl;
        glfwSetTime(0.0);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.Inputs(window);
        camera.updateMatrix(45.0f, 0.1f, 1000.0f);

        terrain.Draw(shaderProgram, camera, objectModel, GL_TRIANGLE_STRIP);
        //light.Draw(lightShader, camera, lightModel);

        const size_t threadCount = std::thread::hardware_concurrency();
        std::vector<std::thread> threads;
        std::vector<std::future<std::vector<Collision>>> futures(threadCount);
        const size_t blockSize = rigidbodies.size() / threadCount;

        // Draw and update rigidbodies
        for (Rigidbody& rb : rigidbodies) {
            rb.oldPosition = rb.position;
            rb.applyForce(rb.mass * glm::vec3(0.0f, -9.8f, 0.0f));
            rb.integrate(1.0f / 60.0f);

            // Check for collision with the terrain
            float xOnTerrain = rb.position.x + ((float) TERRAIN_SIZE / 2);
            float zOnTerrain = rb.position.z + ((float) TERRAIN_SIZE / 2);
            if (xOnTerrain >= 0 && xOnTerrain < TERRAIN_SIZE && zOnTerrain >= 0 && zOnTerrain < TERRAIN_SIZE) {
                float terrainHeight = perlin.noise(xOnTerrain * SCALE, zOnTerrain * SCALE) * TERRAIN_HEIGHT;
                if (rb.position.y - rb.sphere.radius < terrainHeight) {
                    glm::vec3 normal = perlin.normal(xOnTerrain, zOnTerrain, TERRAIN_HEIGHT, SCALE, NORMAL_DELTA);
                    rb.position.y = terrainHeight + rb.sphere.radius;
                    rb.velocity = glm::reflect(rb.velocity, normal);
                }
            }
        }

        for (size_t i = 0; i < threadCount; ++i) {
            size_t start = i * blockSize;
            size_t end = (i == threadCount - 1) ? rigidbodies.size() : start + blockSize;
            std::promise<std::vector<Collision>> p;
            futures[i] = p.get_future();
            threads.emplace_back(findCollisions, std::ref(rigidbodies), std::ref(grid), start, end, std::move(p));
        }


        for (auto& thread : threads) {
            thread.join();
        }


        std::vector<Collision> collisions;
        for (auto& f : futures) {
            std::vector<Collision> result = f.get();
            collisions.insert(collisions.end(), result.begin(), result.end());
        }


        for (Collision& c : collisions) {
            glm::vec3 direction = c.rb2->position - c.rb1->position;
            float distance = glm::length(direction);
            float sumRadii = c.rb1->sphere.radius + c.rb2->sphere.radius;
            glm::vec3 normal = glm::normalize(direction);
            c.rb1->position -= 0.5f * (sumRadii - distance) * normal;
            c.rb2->position += 0.5f * (sumRadii - distance) * normal;


            c.rb1->velocity += c.impulse / c.rb1->mass;
            c.rb2->velocity -= c.impulse / c.rb2->mass;
        }

        std::vector<glm::mat4> instanceMatrix;

        for (Rigidbody& rb : rigidbodies) {
            grid.updateSphere(&rb, rb.oldPosition);


            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, rb.position);
            model = glm::scale(model, glm::vec3(rb.sphere.radius));
            instanceMatrix.push_back(model);
        }
        sphere.Draw(instancingShaderProgram, camera, glm::mat4(1.0f), GL_TRIANGLES, instanceMatrix.size(), instanceMatrix);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    shaderProgram.Delete();
    lightShader.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
