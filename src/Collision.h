# include "Rigidbody.h"
#ifndef OPENGL_TEST_COLLISION_H
#define OPENGL_TEST_COLLISION_H


class Collision {
public:
    Rigidbody* rb1;
    Rigidbody* rb2;
    glm::vec3 impulse;

    Collision(Rigidbody* rb1, Rigidbody* rb2, glm::vec3 impulse);
};


#endif //OPENGL_TEST_COLLISION_H
