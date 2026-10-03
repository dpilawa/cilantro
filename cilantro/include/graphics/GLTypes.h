#pragma once

#include "cilantroengine.h"
#include "glad/gl.h"

namespace cilantro {

enum EGlVBOType { VBO_VERTICES = 0, VBO_NORMALS, VBO_UVS, VBO_TANGENTS, VBO_BITANGENTS, VBO_BONES, VBO_BONEWEIGHTS };
enum EGlUBOType { UBO_MATRICES = 0, UBO_POINTLIGHTS, UBO_DIRECTIONALLIGHTS, UBO_SPOTLIGHTS, UBO_DIRECTIONALLIGHTVIEWMATRICES, UBO_SPOTLIGHTVIEWMATRICES, UBO_POINTLIGHTVIEWMATRICES, UBO_BONETRANSFORMATIONS };
enum EGlSSBOType { SSBO_VERTICES = 0, SSBO_BONEINDICES, SSBO_BONEWEIGHTS, SSBO_AABB };

struct SGlGeometryBuffers
{
    // number of vertices
    size_t indexCount;
    // Vertex Buffer Objects (vertices, normals, uvs, tangents, bitangents, bone indices, bone weights)
    GLuint VBO[CILANTRO_VBO_COUNT];
    // Element Buffer Object (face indices)
    GLuint EBO;
    // Vertex Array Object
    GLuint VAO;
    // Bone transformation buffers
    GLuint vertexPositionsSSBO;
    GLuint boneTransformationsUBO;
    GLuint boneIndicesSSBO;
    GLuint boneWeightsSSBO;
    GLuint aabbSSBO;
};

struct SGlUniformBuffers
{
    // Uniform Buffer Objects (view & projection matrices, point lights, directional lights, spot lights, directional light view transforms, spot light view transforms, point light view transforms, bone transformations)
    GLuint UBO[CILANTRO_GLOBAL_UBO_COUNT];
};

struct SGlUniformMatrixBuffer
{
    // view matrix
    GLfloat viewMatrix[16];
    // projection matrix
    GLfloat projectionMatrix[16];
};

struct SGlUniformLightViewMatrixBuffer
{
    // directional light view matrices
    GLfloat directionalLightView[16 * CILANTRO_MAX_DIRECTIONAL_LIGHTS];
    // spot light view matrices
    GLfloat spotLightView[16 * CILANTRO_MAX_SPOT_LIGHTS];
    // point light view matrices (cube maps)
    GLfloat pointLightView[16 * 6 * CILANTRO_MAX_POINT_LIGHTS];
};

struct SGlMaterialTextureUnits
{
    // how many units in use 
    unsigned int unitsCount;
    // using 16 texture units, as per minimum defined in OpenGL 3.x
    GLuint textureUnits[CILANTRO_MAX_TEXTURE_UNITS];
};

struct SGlPointLightStruct
{
    GLfloat lightPosition[3];
    GLfloat pad1;
    GLfloat lightColor[3];
    GLfloat attenuationConst;
    GLfloat attenuationLinear;
    GLfloat attenuationQuadratic;
};

struct SGlDirectionalLightStruct
{
    GLfloat lightDirection[3];
    GLfloat pad1;
    GLfloat lightColor[3];
    GLfloat pad2;
};

struct SGlSpotLightStruct
{
    GLfloat lightPosition[3];
    GLfloat pad1;
    GLfloat lightDirection[3];
    GLfloat pad2;
    GLfloat lightColor[3];
    GLfloat attenuationConst;
    GLfloat attenuationLinear;
    GLfloat attenuationQuadratic;
    GLfloat innerCutoffCosine;
    GLfloat outerCutoffCosine;
};

struct SGlUniformPointLightBuffer
{
    // number of active point lights
    GLuint pointLightCount;
    // pad to std140 specification
    GLint pad[3];
    // array of active point lights
    SGlPointLightStruct pointLights[CILANTRO_MAX_POINT_LIGHTS];
};

struct SGlUniformDirectionalLightBuffer
{
    // number of active directional lights
    GLuint directionalLightCount;
    // pad to std140 specification
    GLint pad[3];
    // array of active point lights
    SGlDirectionalLightStruct directionalLights[CILANTRO_MAX_DIRECTIONAL_LIGHTS];
};

struct SGlUniformSpotLightBuffer
{
    // number of active spot lights
    GLuint spotLightCount;
    // pad to std140 specification
    GLint pad[3];
    // array of active point lights
    SGlSpotLightStruct spotLights[CILANTRO_MAX_SPOT_LIGHTS];
};

struct SGlEncodedAABB {
    GLuint minBits[3];
    GLuint pad1;
    GLuint maxBits[3];
    GLuint pad2;
};

} // namespace cilantro
