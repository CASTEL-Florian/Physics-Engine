// Rigidbody.h

#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include <glm/glm.hpp> // GLM for vector mathematics
#include "Sphere.h"

class Rigidbody {
public:
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    float mass;
    glm::vec3 forces;
    Sphere sphere;
    int id;
    glm::vec3 oldPosition;

    Rigidbody(glm::vec3 pos = glm::vec3(0.0f), float radius = 1.0f, float mass = 1.0f);

    void applyForce(const glm::vec3& force);
    void integrate(float deltaTime);

private:
    void clearForces();
};

#endif // RIGIDBODY_H

