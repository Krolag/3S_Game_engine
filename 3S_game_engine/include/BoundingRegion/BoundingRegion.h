#pragma once
#ifndef BOUNDINGREGION_H
#define BOUNDINGREGION_H

#include <glm/glm.hpp>
#include <iostream>

enum class BoundTypes : unsigned char {
    AABB = 0x00,	// 0x00 = 0	// Axis-aligned bounding box
    SPHERE = 0x01	// 0x01 = 1
};

class BoundingRegion {
public:
    // type of region
    BoundTypes type;

    // sphere values
    glm::vec3 center;
    float radius;
    glm::vec3 ogCenter;
    float ogRadius;

    // bounding box values
    glm::vec3 min;
    glm::vec3 max;
    glm::vec3 ogMin;
    glm::vec3 ogMax;

    // initialize with type
    BoundingRegion(BoundTypes type = BoundTypes::AABB);
    // initialize as sphere
    BoundingRegion(glm::vec3 center, float radius);
    // initialize as AABB
    BoundingRegion(glm::vec3 min, glm::vec3 max);

    void transform();
    glm::vec3 calculateCenter();
    glm::vec3 calculateDimensions();
    // determine if point inside
    bool containsPoint(glm::vec3 pt);
    // determine if region completely inside
    bool containsRegion(BoundingRegion br);
    // determine if region intersects (partial containment)
    bool intersectsWith(BoundingRegion br);
    // operator overload
    bool operator==(BoundingRegion br);
};

#endif // !BOUNDINGREGION_H