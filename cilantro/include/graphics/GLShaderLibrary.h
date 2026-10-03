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

private:
    void LoadShaders ();
    void CreatePrograms ();

    std::shared_ptr<ResourceManager<Resource>> m_shaderResources;
    std::shared_ptr<TShaderProgramManager> m_shaderPrograms;
};

} // namespace cilantro
