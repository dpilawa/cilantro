#pragma once

#include "cilantroengine.h"
#include "graphics/IRenderer.h"
#include "resource/ResourceManager.h"
#include <memory>

namespace cilantro {

class Resource;

// loads standard shaders and builds standard shader programs used by GL renderer
class GLShaderLibrary
{
public:
    GLShaderLibrary (std::shared_ptr<ResourceManager<Resource>> shaderResources, std::shared_ptr<TShaderProgramManager> shaderPrograms);
    virtual ~GLShaderLibrary ();

    // load shaders to resource manager, create and link shader programs
    void Initialize ();

    // update shadow map shaders after a light of given type has been added, parameters are light counts including the added light
    void OnPointLightAdded (size_t directionalLightCount, size_t spotLightCount, size_t pointLightCount);
    void OnDirectionalLightAdded (size_t directionalLightCount, size_t spotLightCount, size_t pointLightCount);
    void OnSpotLightAdded (size_t directionalLightCount, size_t spotLightCount, size_t pointLightCount);

private:
    void LoadShaders ();
    void CreatePrograms ();

    std::shared_ptr<ResourceManager<Resource>> m_shaderResources;
    std::shared_ptr<TShaderProgramManager> m_shaderPrograms;
};

} // namespace cilantro
