// Rigidbody.cpp

#include "Rigidbody.h"

Rigidbody::Rigidbody(glm::vec3 pos,float radius, float m)
        : position(pos), mass(m), velocity(glm::vec3(0.0f)), acceleration(glm::vec3(0.0f)), forces(glm::vec3(0.0f)), sphere(Sphere(pos.x, pos.y, pos.z, radius)) {
    static int idCounter = 0;
    id = idCounter++;
}

void Rigidbody::applyForce(const glm::vec3& force) {
    forces += force;
}

void Rigidbody::integrate(float deltaTime) {
    if (mass <= 0.0f) return; // Avoid division by zero

    // Newton's second law: F = ma -> a = F/m
    acceleration = forces / mass;

    // Update velocity: v = v0 + at
    velocity += acceleration * deltaTime;

    // Update position: s = s0 + vt
    position += velocity * deltaTime;

    // Clear forces after integration
    clearForces();
}

void Rigidbody::clearForces() {
    forces = glm::vec3(0.0f);
}
