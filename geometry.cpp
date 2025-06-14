#include "geometry.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

bool showRow[3] = { true, true, true };
std::vector<float> vertices = {};
float flowerSize = 0.2f;       // half-width of quad (XZ)
float flowerHeight = 0.2f;     // height of quads
float baseY = 0.05f;
float xOffsets[5] = { -0.8f, -0.4f, 0.0f, 0.4f, 0.8f };
float zOffsets[3] = { -0.8f, -0.4f, 0.0f };
float flowerScale = 0.5f;


glm::vec3 rotateAndScaleAroundX(float x, float y, float z, float angle, float scale) {
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::rotate(transform, angle, glm::vec3(1, 0, 0));
    transform = glm::scale(transform, glm::vec3(scale));
    return glm::vec3(transform * glm::vec4(x, y, z, 1.0f));
}
void generateVertices( float flowerScale, float flowerSize, float flowerHeight, float baseY, float* xOffsets, float* zOffsets) {
    vertices.clear();
    
    auto pushGroundVertex = [&](float x, float y, float z, glm::vec3 color) {
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(z);
        vertices.push_back(color.r);
        vertices.push_back(color.g);
        vertices.push_back(color.b);
        vertices.push_back(1.0f); 
    };
    glm::vec3 grassColor = glm::vec3(0.3f, 0.7f, 0.3f); 
    glm::vec3 dirtColor  = glm::vec3(0.4f, 0.25f, 0.1f);

    float groundTopY = 0.0f;
    float groundBottomY = -0.2f;
    float groundSize = 1.0f;

    // Top face - grass 
    pushGroundVertex(-groundSize, groundTopY,  groundSize, grassColor);
    pushGroundVertex( groundSize, groundTopY,  groundSize, grassColor);
    pushGroundVertex( groundSize, groundTopY, -groundSize, grassColor);
    pushGroundVertex( groundSize, groundTopY, -groundSize, grassColor);
    pushGroundVertex(-groundSize, groundTopY, -groundSize, grassColor);
    pushGroundVertex(-groundSize, groundTopY,  groundSize, grassColor);

    // Bottom face - dirt
    pushGroundVertex(-groundSize, groundBottomY, -groundSize, dirtColor);
    pushGroundVertex( groundSize, groundBottomY, -groundSize, dirtColor);
    pushGroundVertex( groundSize, groundBottomY,  groundSize, dirtColor);
    pushGroundVertex( groundSize, groundBottomY,  groundSize, dirtColor);
    pushGroundVertex(-groundSize, groundBottomY,  groundSize, dirtColor);
    pushGroundVertex(-groundSize, groundBottomY, -groundSize, dirtColor);

    // Side face - +Z
    pushGroundVertex(-groundSize, groundBottomY,  groundSize, dirtColor);
    pushGroundVertex( groundSize, groundBottomY,  groundSize, dirtColor);
    pushGroundVertex( groundSize, groundTopY,     groundSize, dirtColor);

    pushGroundVertex( groundSize, groundTopY,     groundSize, dirtColor);
    pushGroundVertex(-groundSize, groundTopY,     groundSize, dirtColor);
    pushGroundVertex(-groundSize, groundBottomY,  groundSize, dirtColor);

    // Side face - -Z
    pushGroundVertex( groundSize, groundBottomY, -groundSize, dirtColor);
    pushGroundVertex(-groundSize, groundBottomY, -groundSize, dirtColor);
    pushGroundVertex(-groundSize, groundTopY,    -groundSize, dirtColor);

    pushGroundVertex(-groundSize, groundTopY,    -groundSize, dirtColor);
    pushGroundVertex( groundSize, groundTopY,    -groundSize, dirtColor);
    pushGroundVertex( groundSize, groundBottomY, -groundSize, dirtColor);

    // Side face - -X
    pushGroundVertex(-groundSize, groundBottomY, -groundSize, dirtColor);
    pushGroundVertex(-groundSize, groundBottomY,  groundSize, dirtColor);
    pushGroundVertex(-groundSize, groundTopY,     groundSize, dirtColor);

    pushGroundVertex(-groundSize, groundTopY,     groundSize, dirtColor);
    pushGroundVertex(-groundSize, groundTopY,    -groundSize, dirtColor);
    pushGroundVertex(-groundSize, groundBottomY, -groundSize, dirtColor);

    // Side face - +X
    pushGroundVertex( groundSize, groundBottomY,  groundSize, dirtColor);
    pushGroundVertex( groundSize, groundBottomY, -groundSize, dirtColor);
    pushGroundVertex( groundSize, groundTopY,    -groundSize, dirtColor);

    pushGroundVertex( groundSize, groundTopY,    -groundSize, dirtColor);
    pushGroundVertex( groundSize, groundTopY,     groundSize, dirtColor);
    pushGroundVertex( groundSize, groundBottomY,  groundSize, dirtColor);


    for (int zi = 0; zi < 3; zi++) {
        if (!showRow[zi]) continue; 

        float z = zOffsets[zi];
        float angle = glm::radians(20.0f * zi);

        glm::vec3 color1;
        glm::vec3 color2;
        switch (zi) {
            case 0: {
                color1 = glm::vec3(1.0f, 0.4f, 0.4f);
                color2 = glm::vec3(0.8f, 0.8f, 0.3f); 
                break;
            }
            case 1: {
                color1 = glm::vec3(0.8f, 0.3f, 0.4f);
                color2 = glm::vec3(0.9f, 0.9f, 0.7f);
                break;
            }
            case 2: {
                color1 = glm::vec3(0.7f, 0.45f, 0.9f);
                color2 = glm::vec3(0.9f, 0.6f, 0.45f); 
                break;
            }
        }
       
        for (int xi = 0; xi < 5; xi++) {
            float x = xOffsets[xi];

            glm::vec3 v0 = rotateAndScaleAroundX(-flowerSize, 0.0f, 0.0f, angle, flowerScale);
            glm::vec3 v1 = rotateAndScaleAroundX(flowerSize, 0.0f, 0.0f, angle, flowerScale);
            glm::vec3 v2 = rotateAndScaleAroundX(flowerSize, flowerHeight, 0.0f, angle, flowerScale);
            glm::vec3 v3 = rotateAndScaleAroundX(-flowerSize, flowerHeight, 0.0f, angle, flowerScale);

            glm::vec3 center(x, baseY, z);

            auto pushXZVertex = [&](glm::vec3 pos) {
                vertices.push_back(center.x + pos.x);
                vertices.push_back(center.y + pos.y);
                vertices.push_back(center.z + pos.z);
                vertices.push_back(color1.r); vertices.push_back(color1.g); vertices.push_back(color1.b); vertices.push_back(1.0f);
            };

            pushXZVertex(v0); pushXZVertex(v1); pushXZVertex(v2);
            pushXZVertex(v2); pushXZVertex(v3); pushXZVertex(v0);

            float px = x;
            float pz = z;
            float sy = baseY;

            auto pushZY = [&](float dx, float dy, float dz) {
                vertices.push_back(px + dx * flowerScale);
                vertices.push_back(sy + dy * flowerScale);
                vertices.push_back(pz + dz * flowerScale);
                vertices.push_back(color2.r); vertices.push_back(color2.g); vertices.push_back(color2.b); vertices.push_back(1.0f);
            };

            pushZY(0, 0, -flowerSize);
            pushZY(0, 0, flowerSize);
            pushZY(0, flowerHeight, flowerSize);

            pushZY(0, flowerHeight, flowerSize);
            pushZY(0, flowerHeight, -flowerSize);
            pushZY(0, 0, -flowerSize);
        }
    }
}
