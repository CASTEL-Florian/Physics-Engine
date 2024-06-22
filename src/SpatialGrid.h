#ifndef OPENGL_TEST_SPATIALGRID_H
#define OPENGL_TEST_SPATIALGRID_H

#include <vector>
#include <unordered_map>
#include <cmath>
#include "Rigidbody.h"

class SpatialGrid {
public:
    SpatialGrid(float cellSize);

    void addSphere(Rigidbody* rb);
    void updateSphere(Rigidbody* rb, glm::vec3 oldPosition);
    std::vector<Rigidbody*> getPotentialCollisions(Rigidbody* rb);

private:
    float cellSize;
    int gridWidth, gridHeight, gridDepth;
    std::unordered_map<int, std::vector<Rigidbody*>> grid;

    int computeHash(int x, int y, int z) const;
    std::tuple<int, int, int> getGridCoordinates(float x, float y, float z) const;
};

#endif //OPENGL_TEST_SPATIALGRID_H
