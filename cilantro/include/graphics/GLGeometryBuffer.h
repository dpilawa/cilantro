#ifndef _GLGEOMETRYBUFFER_H_
#define _GLGEOMETRYBUFFER_H_

#include "cilantroengine.h"
#include "graphics/GeometryBuffer.h"

class GLGeometryBuffer : public GeometryBuffer
{
public:
    // Vertex Buffer Objects (vertices, normals, uvs, tangents, bitangents, bone indices, bone weights)
    GLuint VBO[CILANTRO_VBO_COUNT];
    // Element Buffer Object (face indices)
    GLuint EBO;
    // Vertex Array Object
    GLuint VAO;
};

#endif