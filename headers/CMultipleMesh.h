#ifndef CMULTIPLEMESH_H
#define CMULTIPLEMESH_H

#include "CMesh.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

class CMultipleMesh : public CMesh {
public:
    GLuint instanceCount;
    std::vector<glm::mat4> instanceTransformations;

    CMultipleMesh() : CMesh(), instanceCount(0) {
    }

    CMultipleMesh(GLuint instanceCount) : CMesh(), instanceCount(instanceCount) {
        instanceTransformations.resize(instanceCount);
    }

    void SetInstanceTransformation(GLuint instanceIndex, const glm::mat4& transformation) {
        if (instanceIndex < instanceCount) {
            instanceTransformations[instanceIndex] = transformation;
        }
    }

    void DrawInstances() {
        glBindVertexArray(idVAO);
        glDrawArraysInstanced( GL_TRIANGLES, 0, GetVertexCount(), instanceCount );
        glBindVertexArray(0);
    }
};



#endif // CMULTIPLEMESH_H
