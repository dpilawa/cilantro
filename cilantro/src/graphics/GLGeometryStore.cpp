#include "graphics/GLGeometryStore.h"
#include "graphics/GLUtils.h"
#include "graphics/GLShaderProgram.h"
#include "graphics/IShaderProgram.h"
#include "resource/Mesh.h"
#include "scene/GameObject.h"
#include "scene/MeshObject.h"
#include "math/AABB.h"
#include "math/Matrix4f.h"
#include "math/Vector3f.h"
#include "system/LogMessage.h"
#include <bit>
#include <cstdint>

namespace cilantro {

GLGeometryStore::GLGeometryStore ()
    : m_surfaceGeometryBuffer ()
{
}

GLGeometryStore::~GLGeometryStore ()
{
}

void GLGeometryStore::Update (std::shared_ptr<MeshObject> meshObject)
{
    handle_t objectHandle = meshObject->GetHandle ();

    // check of object's buffers are already initialized
    auto find = m_sceneGeometryBuffers.find (objectHandle);

    if (find == m_sceneGeometryBuffers.end ())
    {
        // it is a new object, so generate buffers 
        SGlGeometryBuffers& b = m_sceneGeometryBuffers[objectHandle];

        // generate and bind Vertex Array Object (VAO)
        glGenVertexArrays (1, &b.VAO);
        glBindVertexArray (b.VAO);

        // generate vertex buffer
        glGenBuffers (1, &b.VBO[EGlVBOType::VBO_VERTICES]);
        glBindBuffer (GL_ARRAY_BUFFER, b.VBO[EGlVBOType::VBO_VERTICES]);
        // location = 0 (vertex position)
        glVertexAttribPointer (EGlVBOType::VBO_VERTICES, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (GLvoid*)0);

        // generate normals buffer
        glGenBuffers (1, &b.VBO[EGlVBOType::VBO_NORMALS]);
        glBindBuffer (GL_ARRAY_BUFFER, b.VBO[EGlVBOType::VBO_NORMALS]);
        // location = 1 (vertex normal)
        glVertexAttribPointer (EGlVBOType::VBO_NORMALS, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (GLvoid*)0);

        // generate uv buffer
        glGenBuffers (1, &b.VBO[EGlVBOType::VBO_UVS]);
        glBindBuffer (GL_ARRAY_BUFFER, b.VBO[EGlVBOType::VBO_UVS]);
        // location = 2 (vertex uv)
        glVertexAttribPointer (EGlVBOType::VBO_UVS, 2, GL_FLOAT, GL_FALSE, 2 * sizeof (float), (GLvoid*)0);

        // generate tangents buffer
        glGenBuffers (1, &b.VBO[EGlVBOType::VBO_TANGENTS]);
        glBindBuffer (GL_ARRAY_BUFFER, b.VBO[EGlVBOType::VBO_TANGENTS]);
        // location = 3 (vertex tangent)
        glVertexAttribPointer (EGlVBOType::VBO_TANGENTS, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (GLvoid*)0);

        // generate bitangents buffer
        glGenBuffers (1, &b.VBO[EGlVBOType::VBO_BITANGENTS]);
        glBindBuffer (GL_ARRAY_BUFFER, b.VBO[EGlVBOType::VBO_BITANGENTS]);
        // location = 4 (vertex bitangent)
        glVertexAttribPointer (EGlVBOType::VBO_BITANGENTS, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (GLvoid*)0);

        // generate bone index
        glGenBuffers (1, &b.VBO[EGlVBOType::VBO_BONES]);
        glBindBuffer (GL_ARRAY_BUFFER, b.VBO[EGlVBOType::VBO_BONES]);
        // location = 5 (bone indices)
        glVertexAttribIPointer (EGlVBOType::VBO_BONES, CILANTRO_MAX_BONE_INFLUENCES, GL_INT, CILANTRO_MAX_BONE_INFLUENCES * sizeof (int), (GLvoid*)0);

        // generate bone weight buffer
        glGenBuffers (1, &b.VBO[EGlVBOType::VBO_BONEWEIGHTS]);
        glBindBuffer (GL_ARRAY_BUFFER, b.VBO[EGlVBOType::VBO_BONEWEIGHTS]);
        // location = 6 (bone weights)
        glVertexAttribPointer (EGlVBOType::VBO_BONEWEIGHTS, CILANTRO_MAX_BONE_INFLUENCES, GL_FLOAT, GL_FALSE, CILANTRO_MAX_BONE_INFLUENCES * sizeof (int), (GLvoid*)0);

        // generate bone transformation matrix array uniform buffer
        glGenBuffers (1, &b.boneTransformationsUBO);
        glBindBuffer (GL_UNIFORM_BUFFER, b.boneTransformationsUBO);
        glBufferData (GL_UNIFORM_BUFFER, CILANTRO_MAX_BONES * sizeof (GLfloat) * 16, NULL, GL_DYNAMIC_DRAW);
        glBindBufferBase (GL_UNIFORM_BUFFER, static_cast<int>(EGlUBOType::UBO_BONETRANSFORMATIONS), b.boneTransformationsUBO);


        if (GLUtils::GetGLSLVersion ().versionNumber >= 430)
        {
            // generate vertex positions array SSBO buffer
            glGenBuffers (1, &b.vertexPositionsSSBO);
            glBindBuffer (GL_SHADER_STORAGE_BUFFER, b.vertexPositionsSSBO);
            glBufferData (GL_SHADER_STORAGE_BUFFER, CILANTRO_MAX_VERTICES * sizeof (GLfloat) * 3, NULL, GL_DYNAMIC_DRAW);
            glBindBufferBase (GL_SHADER_STORAGE_BUFFER, static_cast<int>(EGlSSBOType::SSBO_VERTICES), b.vertexPositionsSSBO);

            // generate bone indices array SSBO buffer
            glGenBuffers (1, &b.boneIndicesSSBO);
            glBindBuffer (GL_SHADER_STORAGE_BUFFER, b.boneIndicesSSBO);
            glBufferData (GL_SHADER_STORAGE_BUFFER, CILANTRO_MAX_VERTICES * sizeof (GLuint) * CILANTRO_MAX_BONE_INFLUENCES, NULL, GL_DYNAMIC_DRAW);
            glBindBufferBase (GL_SHADER_STORAGE_BUFFER, static_cast<int>(EGlSSBOType::SSBO_BONEINDICES), b.boneIndicesSSBO);

            // generate bone weights array SSBO buffer
            glGenBuffers (1, &b.boneWeightsSSBO);
            glBindBuffer (GL_SHADER_STORAGE_BUFFER, b.boneWeightsSSBO);
            glBufferData (GL_SHADER_STORAGE_BUFFER, CILANTRO_MAX_VERTICES * sizeof (GLfloat) * CILANTRO_MAX_BONE_INFLUENCES, NULL, GL_DYNAMIC_DRAW);
            glBindBufferBase (GL_SHADER_STORAGE_BUFFER, static_cast<int>(EGlSSBOType::SSBO_BONEWEIGHTS), b.boneWeightsSSBO);

            // generate AABB result SSBO buffer
            glGenBuffers (1, &b.aabbSSBO);
            glBindBuffer (GL_SHADER_STORAGE_BUFFER, b.aabbSSBO);
            glBufferData (GL_SHADER_STORAGE_BUFFER, sizeof (SGlEncodedAABB), NULL, GL_DYNAMIC_DRAW);
            glBindBufferBase (GL_SHADER_STORAGE_BUFFER, static_cast<int>(EGlSSBOType::SSBO_AABB), b.aabbSSBO);
        }

        // generate index buffer
        glGenBuffers (1, &b.EBO);
        glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, b.EBO);

        // enable VBO arrays
        glEnableVertexAttribArray (EGlVBOType::VBO_VERTICES);
        glEnableVertexAttribArray (EGlVBOType::VBO_NORMALS);
        glEnableVertexAttribArray (EGlVBOType::VBO_UVS);
        glEnableVertexAttribArray (EGlVBOType::VBO_TANGENTS);
        glEnableVertexAttribArray (EGlVBOType::VBO_BITANGENTS);
        glEnableVertexAttribArray (EGlVBOType::VBO_BONES);
        glEnableVertexAttribArray (EGlVBOType::VBO_BONEWEIGHTS);

        // unbind VAO
        glBindVertexArray (0);

    }

    // resize buffers and load data
    SGlGeometryBuffers& b = m_sceneGeometryBuffers[objectHandle];
    b.indexCount = meshObject->GetMesh ()->GetIndexCount ();

    // bind Vertex Array Object (VAO)
    glBindVertexArray (b.VAO);

    // load vertex buffer
    glBindBuffer (GL_ARRAY_BUFFER, b.VBO[EGlVBOType::VBO_VERTICES]);
    glBufferData (GL_ARRAY_BUFFER, meshObject->GetMesh ()->GetVertexCount () * sizeof (float) * 3, meshObject->GetMesh ()->GetVerticesData (), GL_DYNAMIC_DRAW);

    // load normals buffer
    glBindBuffer (GL_ARRAY_BUFFER, b.VBO[EGlVBOType::VBO_NORMALS]);
    glBufferData (GL_ARRAY_BUFFER, meshObject->GetMesh ()->GetVertexCount () * sizeof (float) * 3, meshObject->GetMesh ()->GetNormalsData (), GL_DYNAMIC_DRAW);
    
    // load uv buffer
    glBindBuffer (GL_ARRAY_BUFFER, b.VBO[EGlVBOType::VBO_UVS]);
    glBufferData (GL_ARRAY_BUFFER, meshObject->GetMesh ()->GetVertexCount () * sizeof (float) * 2, meshObject->GetMesh ()->GetUVData (), GL_DYNAMIC_DRAW);

    // load tangents buffer
    glBindBuffer (GL_ARRAY_BUFFER, b.VBO[EGlVBOType::VBO_TANGENTS]);
    glBufferData (GL_ARRAY_BUFFER, meshObject->GetMesh ()->GetVertexCount () * sizeof (float) * 3, meshObject->GetMesh ()->GetTangentData (), GL_DYNAMIC_DRAW);

    // load bitangents buffer
    glBindBuffer (GL_ARRAY_BUFFER, b.VBO[EGlVBOType::VBO_BITANGENTS]);
    glBufferData (GL_ARRAY_BUFFER, meshObject->GetMesh ()->GetVertexCount () * sizeof (float) * 3, meshObject->GetMesh ()->GetBitangentData (), GL_DYNAMIC_DRAW);

    // load bone index buffer
    glBindBuffer (GL_ARRAY_BUFFER, b.VBO[EGlVBOType::VBO_BONES]);
    glBufferData (GL_ARRAY_BUFFER, meshObject->GetMesh ()->GetVertexCount () * sizeof (uint32_t) * CILANTRO_MAX_BONE_INFLUENCES, meshObject->GetMesh ()->GetBoneIndicesData (), GL_DYNAMIC_DRAW);

    // load bone weight buffer
    glBindBuffer (GL_ARRAY_BUFFER, b.VBO[EGlVBOType::VBO_BONEWEIGHTS]);
    glBufferData (GL_ARRAY_BUFFER, meshObject->GetMesh ()->GetVertexCount () * sizeof (float) * CILANTRO_MAX_BONE_INFLUENCES, meshObject->GetMesh ()->GetBoneWeightsData (), GL_DYNAMIC_DRAW);

    // load index buffer
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, b.EBO);
    glBufferData (GL_ELEMENT_ARRAY_BUFFER, meshObject->GetMesh ()->GetIndexCount () * sizeof (uint32_t), meshObject->GetMesh ()->GetFacesData (), GL_DYNAMIC_DRAW);

    // unbind VAO
    glBindVertexArray (0);

}

void GLGeometryStore::UpdateAABB (std::shared_ptr<MeshObject> meshObject)
{
    handle_t objectHandle = meshObject->GetHandle ();

    // check of object's buffers are already initialized
    auto find = m_aabbGeometryBuffers.find (objectHandle);

    if (find == m_aabbGeometryBuffers.end ())
    {
        // it is a new object, so generate buffers 
        SGlGeometryBuffers& w = m_aabbGeometryBuffers[objectHandle];
        w.indexCount = 12; // AABB has 12 edges

        // generate and bind Vertex Array Object (VAO) - wireframes
        glGenVertexArrays (1, &w.VAO);
        glBindVertexArray (w.VAO);

        // generate vertex buffer - wireframes
        glGenBuffers (1, &w.VBO[EGlVBOType::VBO_VERTICES]);
        glBindBuffer (GL_ARRAY_BUFFER, w.VBO[EGlVBOType::VBO_VERTICES]);
        // location = 0 (vertex position)
        glVertexAttribPointer (EGlVBOType::VBO_VERTICES, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (GLvoid*)0);

        // generate index buffer
        glGenBuffers (1, &w.EBO);
        glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, w.EBO);

        // load index buffer - wireframes (this is static)
        glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, w.EBO);
        glBufferData (GL_ELEMENT_ARRAY_BUFFER, 12 * 2 * sizeof (uint32_t), meshObject->GetAABB ().GetLineIndicesData (), GL_STATIC_DRAW);

        // enable VBO arrays
        glEnableVertexAttribArray (EGlVBOType::VBO_VERTICES);

        // unbind VAO
        glBindVertexArray (0);

    }

    // reload data
    SGlGeometryBuffers& w = m_aabbGeometryBuffers[objectHandle];

    // bind Vertex Array Object (VAO) - wireframes
    glBindVertexArray (w.VAO);

    // load vertex buffer - wireframes
    glBindBuffer (GL_ARRAY_BUFFER, w.VBO[EGlVBOType::VBO_VERTICES]);
    glBufferData (GL_ARRAY_BUFFER, 8 * sizeof (float) * 3, meshObject->GetAABB ().GetVerticesData () , GL_DYNAMIC_DRAW);
    
    // unbind VAO - wireframes
    glBindVertexArray (0);

}

AABB GLGeometryStore::CalculateAABB (std::shared_ptr<MeshObject> meshObject, std::shared_ptr<GLShaderProgram> computeShader)
{
    AABB aabb;
    SGlEncodedAABB aabbGPU;
    SGlGeometryBuffers& b = GetBuffers (meshObject);

    computeShader->Use ();
    
    // get world matrix for drawn objects and set uniform value
    computeShader->SetUniformMatrix4f ("mModel", meshObject->GetWorldTransformMatrix ());

    LoadBoneTransformations (b, meshObject);

    // load vertex positions array buffer (SSBO)
    glBindBuffer (GL_SHADER_STORAGE_BUFFER, b.vertexPositionsSSBO);
    glBufferData (GL_SHADER_STORAGE_BUFFER, meshObject->GetMesh ()->GetVertexCount () * sizeof (GLfloat) * 3, meshObject->GetMesh ()->GetVerticesData (), GL_DYNAMIC_DRAW);
    glBindBufferBase (GL_SHADER_STORAGE_BUFFER, static_cast<int>(EGlSSBOType::SSBO_VERTICES), b.vertexPositionsSSBO);

    // load bone indices array buffer (SSBO)
    glBindBuffer (GL_SHADER_STORAGE_BUFFER, b.boneIndicesSSBO);
    glBufferData (GL_SHADER_STORAGE_BUFFER, meshObject->GetMesh ()->GetVertexCount () * sizeof (GLuint) * CILANTRO_MAX_BONE_INFLUENCES, meshObject->GetMesh ()->GetBoneIndicesData (), GL_DYNAMIC_DRAW);
    glBindBufferBase (GL_SHADER_STORAGE_BUFFER, static_cast<int>(EGlSSBOType::SSBO_BONEINDICES), b.boneIndicesSSBO);

    // load bone weights array buffer (SSBO)
    glBindBuffer (GL_SHADER_STORAGE_BUFFER, b.boneWeightsSSBO);
    glBufferData (GL_SHADER_STORAGE_BUFFER, meshObject->GetMesh ()->GetVertexCount () * sizeof (GLfloat) * CILANTRO_MAX_BONE_INFLUENCES, meshObject->GetMesh ()->GetBoneWeightsData (), GL_DYNAMIC_DRAW);
    glBindBufferBase (GL_SHADER_STORAGE_BUFFER, static_cast<int>(EGlSSBOType::SSBO_BONEWEIGHTS), b.boneWeightsSSBO);

    // initialize AABB extreme values
    aabbGPU.minBits[0] = 0xFFFFFFFF;
    aabbGPU.minBits[1] = 0xFFFFFFFF;
    aabbGPU.minBits[2] = 0xFFFFFFFF;
    aabbGPU.pad1 = 0x00000000;
    aabbGPU.maxBits[0] = 0x00000000;
    aabbGPU.maxBits[1] = 0x00000000;
    aabbGPU.maxBits[2] = 0x00000000;
    aabbGPU.pad2 = 0x00000000;
    glBindBuffer (GL_SHADER_STORAGE_BUFFER, b.aabbSSBO);
    glBufferData (GL_SHADER_STORAGE_BUFFER, sizeof (SGlEncodedAABB), &aabbGPU, GL_DYNAMIC_DRAW);
    glBindBufferBase (GL_SHADER_STORAGE_BUFFER, static_cast<int>(EGlSSBOType::SSBO_AABB), b.aabbSSBO);

    // dispatch compute shader
    GLuint groupSize = (static_cast<GLuint>(meshObject->GetMesh ()->GetVertexCount ()) + CILANTRO_COMPUTE_GROUP_SIZE - 1) / CILANTRO_COMPUTE_GROUP_SIZE;
    computeShader->Compute (groupSize, 1, 1);

    // read back AABB from compute shader
    glBindBuffer (GL_SHADER_STORAGE_BUFFER, b.aabbSSBO);
    glGetBufferSubData (GL_SHADER_STORAGE_BUFFER, 0, sizeof (SGlEncodedAABB), &aabbGPU);
    
    // redo the bit flip for the float representation
    auto toFloat = [](std::uint32_t u) -> float {
        std::uint32_t bits = (u & 0x80000000u)
            ? (u & 0x7FFFFFFFu)
            : ~u;
        return std::bit_cast<float>(bits);
    };

    // create the AABB object
    aabb.AddVertex (Vector3f (toFloat (aabbGPU.minBits[0]), toFloat (aabbGPU.minBits[1]), toFloat (aabbGPU.minBits[2])));
    aabb.AddVertex (Vector3f (toFloat (aabbGPU.maxBits[0]), toFloat (aabbGPU.maxBits[1]), toFloat (aabbGPU.maxBits[2])));

    return aabb;
}

void GLGeometryStore::Initialize ()
{
    // set up VBO and VAO
    float quadVertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f
    };

    float quadUV[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f
    };

    GLuint quadIndices[] = {
        0, 1, 2,
        2, 1, 3
    };

    glGenVertexArrays (1, &m_surfaceGeometryBuffer.VAO);    
    glBindVertexArray (m_surfaceGeometryBuffer.VAO);

    glGenBuffers (1, &m_surfaceGeometryBuffer.VBO[EGlVBOType::VBO_VERTICES]);
    glGenBuffers (1, &m_surfaceGeometryBuffer.VBO[EGlVBOType::VBO_UVS]);

    glBindBuffer (GL_ARRAY_BUFFER, m_surfaceGeometryBuffer.VBO[EGlVBOType::VBO_VERTICES]);
    glBufferData (GL_ARRAY_BUFFER, sizeof (quadVertices), &quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer (EGlVBOType::VBO_VERTICES, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindBuffer (GL_ARRAY_BUFFER, m_surfaceGeometryBuffer.VBO[EGlVBOType::VBO_UVS]);
    glBufferData (GL_ARRAY_BUFFER, sizeof (quadUV), &quadUV, GL_STATIC_DRAW);
    glVertexAttribPointer (EGlVBOType::VBO_UVS, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glEnableVertexAttribArray (EGlVBOType::VBO_VERTICES);
    glEnableVertexAttribArray (EGlVBOType::VBO_UVS);

    glGenBuffers (1, &m_surfaceGeometryBuffer.EBO);
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, m_surfaceGeometryBuffer.EBO);
    glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof (quadIndices), &quadIndices, GL_STATIC_DRAW);

    glBindBuffer (GL_ARRAY_BUFFER, 0);
    glBindVertexArray (0);    

    m_surfaceGeometryBuffer.indexCount = 6;

    GLUtils::CheckGLError (MSG_LOCATION);
}


void GLGeometryStore::Deinitialize ()
{
    // surface
    glDeleteVertexArrays(1, &m_surfaceGeometryBuffer.VAO);
    glDeleteBuffers(1, &m_surfaceGeometryBuffer.VBO[EGlVBOType::VBO_VERTICES]);

    // scene objects
    for (auto&& buffer : m_sceneGeometryBuffers)
    {
        glDeleteBuffers (1, &buffer.second.boneTransformationsUBO);

        if (GLUtils::GetGLSLVersion ().versionNumber >= 430)
        {
            glDeleteBuffers (1, &buffer.second.vertexPositionsSSBO);
            glDeleteBuffers (1, &buffer.second.boneIndicesSSBO);
            glDeleteBuffers (1, &buffer.second.boneWeightsSSBO);
            glDeleteBuffers (1, &buffer.second.aabbSSBO);
        }

        glDeleteBuffers (CILANTRO_VBO_COUNT, buffer.second.VBO);
        glDeleteBuffers (1, &buffer.second.EBO);
        glDeleteVertexArrays (1, &buffer.second.VAO);
    }
}

void GLGeometryStore::Draw (std::shared_ptr<MeshObject> meshObject)
{
    SGlGeometryBuffers& b = GetBuffers (meshObject);

    LoadBoneTransformations (b, meshObject);
    RenderGeometryBuffer (b, GL_TRIANGLES);
}

void GLGeometryStore::DrawSurface ()
{
    RenderGeometryBuffer (m_surfaceGeometryBuffer, GL_TRIANGLES);
}

void GLGeometryStore::DrawAll (std::shared_ptr<IShaderProgram> shader, std::shared_ptr<ResourceManager<GameObject>> gameObjects)
{
    shader->Use ();

    for (auto&& geometryBuffer : m_sceneGeometryBuffers)
    {
        auto m = gameObjects->GetByHandle<MeshObject> (geometryBuffer.first);

        // load model matrix to currently bound shader
        shader->SetUniformMatrix4f ("mModel", m->GetWorldTransformMatrix ());

        // load bone transformations and draw
        LoadBoneTransformations (geometryBuffer.second, m);
        RenderGeometryBuffer (geometryBuffer.second, GL_TRIANGLES);
    }
}

void GLGeometryStore::DrawAABBs (std::shared_ptr<IShaderProgram> shader)
{
    shader->Use ();

    for (auto&& geometryBuffer : m_aabbGeometryBuffers)
    {
        RenderGeometryBuffer (geometryBuffer.second, GL_LINES);
    }
}

SGlGeometryBuffers& GLGeometryStore::GetBuffers (std::shared_ptr<MeshObject> meshObject)
{
    auto find = m_sceneGeometryBuffers.find (meshObject->GetHandle ());

    if (find == m_sceneGeometryBuffers.end ())
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Missing geometry buffers for object" << meshObject->GetHandle ();
    }

    return find->second;
}

void GLGeometryStore::LoadBoneTransformations (const SGlGeometryBuffers& buffers, std::shared_ptr<MeshObject> meshObject)
{
    // load bone transformation matrix array to buffer
    glBindBuffer (GL_UNIFORM_BUFFER, buffers.boneTransformationsUBO);
    glBufferData (GL_UNIFORM_BUFFER, CILANTRO_MAX_BONES * sizeof (GLfloat) * 16, meshObject->GetBoneTransformationsMatrixArray (true), GL_DYNAMIC_DRAW);
    glBindBufferBase (GL_UNIFORM_BUFFER, static_cast<int>(EGlUBOType::UBO_BONETRANSFORMATIONS), buffers.boneTransformationsUBO);
}

void GLGeometryStore::RenderGeometryBuffer (const SGlGeometryBuffers& buffer, GLuint type)
{
    // bind
    glBindVertexArray (buffer.VAO);

    // draw
    glDrawElements (type, static_cast<GLsizei> (buffer.indexCount) * sizeof (GLuint), GL_UNSIGNED_INT, 0);

    // unbind
    glBindVertexArray (0);
}

} // namespace cilantro
