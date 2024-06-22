#include "SpatialGrid.h"
#include <algorithm>

SpatialGrid::SpatialGrid(float cellSize)
        : cellSize(cellSize) {
}

void SpatialGrid::addSphere(Rigidbody* rb) {
    auto [gx, gy, gz] = getGridCoordinates(rb->position.x, rb->position.y, rb->position.z);
    int hash = computeHash(gx, gy, gz);
    grid[hash].push_back(rb);
}


void SpatialGrid::updateSphere(Rigidbody* rb, glm::vec3 oldPosition) {
    // Remove the sphere from its current cell
    auto [oldGx, oldGy, oldGz] = getGridCoordinates(oldPosition.x, oldPosition.y, oldPosition.z);
    int oldHash = computeHash(oldGx, oldGy, oldGz);
    auto& oldCell = grid[oldHash];
    oldCell.erase(std::remove(oldCell.begin(), oldCell.end(), rb), oldCell.end());

    // Add the sphere to its new cell
    auto [newGx, newGy, newGz] = getGridCoordinates(rb->position.x, rb->position.y, rb->position.z);
    int newHash = computeHash(newGx, newGy, newGz);
    grid[newHash].push_back(rb);
}

std::vector<Rigidbody*> SpatialGrid::getPotentialCollisions(Rigidbody* rb) {
    std::vector<Rigidbody*> potentialCollisions;
    auto [gx, gy, gz] = getGridCoordinates(rb->position.x, rb->position.y, rb->position.z);

    for (int x = gx - 1; x <= gx + 1; ++x) {
        for (int y = gy - 1; y <= gy + 1; ++y) {
            for (int z = gz - 1; z <= gz + 1; ++z) {
                int hash = computeHash(x, y, z);
                if (grid.find(hash) != grid.end()) {
                    potentialCollisions.insert(potentialCollisions.end(), grid[hash].begin(), grid[hash].end());
                }
            }
        }
    }

    return potentialCollisions;
}

int SpatialGrid::computeHash(int x, int y, int z) const {
    return (x * 73856093) ^ (y * 19349663) ^ (z * 83492791);
}

std::tuple<int, int, int> SpatialGrid::getGridCoordinates(float x, float y, float z) const {
    int gx = static_cast<int>(std::floor(x / cellSize));
    int gy = static_cast<int>(std::floor(y / cellSize));
    int gz = static_cast<int>(std::floor(z / cellSize));
    return std::make_tuple(gx, gy, gz);
}