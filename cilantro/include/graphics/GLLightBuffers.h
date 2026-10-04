#pragma once

#include "cilantroengine.h"
#include "graphics/GLTypes.h"
#include "resource/ResourceManager.h"
#include "math/Vector3f.h"
#include <array>
#include <memory>
#include <unordered_map>

namespace cilantro {

class GameObject;
class PointLight;
class DirectionalLight;
class SpotLight;
class AABB;

typedef std::unordered_map <handle_t, size_t> TLightHandleIdxMap;

// uniform buffers with lights (point, directional, spot) and light view matrices (used by shadow mapping)
// CPU side state (e.g. light counts) is valid right after construction, GL buffers exist between Initialize and Deinitialize
class GLLightBuffers
{
public:
    GLLightBuffers ();
    virtual ~GLLightBuffers ();

    // create and bind GL buffers
    void Initialize ();
    void Deinitialize ();

    // add or update a light, returns true if the light has just been added
    bool Update (std::shared_ptr<PointLight> pointLight);
    bool Update (std::shared_ptr<DirectionalLight> directionalLight);
    bool Update (std::shared_ptr<SpotLight> spotLight);

    // calculate light view matrices for all lights and load them to GPU
    void LoadLightViewMatrices (std::shared_ptr<ResourceManager<GameObject>> gameObjects, const std::array<Vector3f, 8>& frustumVertices, const AABB& sceneAABB);

    size_t GetPointLightCount () const;
    size_t GetDirectionalLightCount () const;
    size_t GetSpotLightCount () const;

private:
    // GL buffers
    GLuint m_pointLightsUBO;
    GLuint m_directionalLightsUBO;
    GLuint m_spotLightsUBO;
    GLuint m_directionalLightViewMatricesUBO;
    GLuint m_spotLightViewMatricesUBO;
    GLuint m_pointLightViewMatricesUBO;

    // data structures for uniforms
    SGlUniformLightViewMatrixBuffer m_lightViewMatrixBuffer;
    SGlUniformPointLightBuffer m_pointLightBuffer;
    SGlUniformDirectionalLightBuffer m_directionalLightBuffer;
    SGlUniformSpotLightBuffer m_spotLightBuffer;

    // maps light object handle to index in uniform buffer
    TLightHandleIdxMap m_pointLights;
    TLightHandleIdxMap m_directionalLights;
    TLightHandleIdxMap m_spotLights;
};

} // namespace cilantro
