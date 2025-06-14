#pragma once

#include <vector>
#include <glm/glm.hpp>

glm::vec3 rotateAndScaleAroundX(float x, float y, float z, float angle, float scale);
void generateVertices( float flowerScale, float flowerSize, float flowerHeight, float baseY, float* xOffsets, float* zOffsets);


extern bool showRow[3];
extern std::vector<float> vertices;
extern float flowerSize;
extern float flowerHeight;
extern float baseY;

extern float xOffsets[5];
extern float zOffsets[3];
extern float flowerScale;
