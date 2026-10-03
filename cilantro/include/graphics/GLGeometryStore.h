#pragma once

#include "cilantroengine.h"
#include "graphics/GLTypes.h"
#include "resource/ResourceManager.h"
#include <memory>
#include <unordered_map>

namespace cilantro {

struct IShaderProgram;
class GLShaderProgram;
class GameObject;
class MeshObject;
class AABB;

// GL geometry buffers of scene meshes, their bounding boxes and a fullscreen surface
class GLGeometryStore
{
public:
    GLGeometryStore ();
    virtual ~GLGeometryStore ();

    // create and delete GL buffers of the fullscreen surface, delete GL buffers of all meshes
    void Initialize ();
    void Deinitialize ();

    // create (for a new object) or reload buffers of a mesh object
    void Update (std::shared_ptr<MeshObject> meshObject);

    // create (for a new object) or reload wireframe buffers of a mesh object's AABB
    void UpdateAABB (std::shared_ptr<MeshObject> meshObject);

    // calculate AABB of a mesh object using GPU compute shader
    AABB CalculateAABB (std::shared_ptr<MeshObject> meshObject, std::shared_ptr<GLShaderProgram> computeShader);

    // draw a single mesh object using currently bound shader
    void Draw (std::shared_ptr<MeshObject> meshObject);

    // draw all mesh objects (setting model matrix in the shader), AABB wireframes or the fullscreen surface
    void DrawAll (std::shared_ptr<IShaderProgram> shader, std::shared_ptr<ResourceManager<GameObject>> gameObjects);
    void DrawAABBs (std::shared_ptr<IShaderProgram> shader);
    void DrawSurface ();

private:
    SGlGeometryBuffers& GetBuffers (std::shared_ptr<MeshObject> meshObject);
    void LoadBoneTransformations (const SGlGeometryBuffers& buffers, std::shared_ptr<MeshObject> meshObject);
    void RenderGeometryBuffer (const SGlGeometryBuffers& buffer, GLuint type);

    // buffers with geometry data to be passed to GPU (key is object handle)
    std::unordered_map <handle_t, SGlGeometryBuffers> m_sceneGeometryBuffers;
    std::unordered_map <handle_t, SGlGeometryBuffers> m_aabbGeometryBuffers;
    SGlGeometryBuffers m_surfaceGeometryBuffer;
};

} // namespace cilantro
