#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "cilantroengine.h"
#include "system/CallbackProvider.h"
#include "resource/ResourceManager.h"
#include "game/GameComposite.h"
#include "scene/GameScene.h"
#include "scene/MeshObject.h"
#include "scene/PointLight.h"
#include "scene/DirectionalLight.h"
#include "scene/SpotLight.h"
#include "scene/Material.h"
#include "graphics/Shader.h"
#include "graphics/RenderTarget.h"
#include "graphics/Postprocess.h"
#include <string>

class Renderer : public GameComposite
{
public:
    __EAPI Renderer (unsigned int width, unsigned int height);
    __EAPI virtual ~Renderer ();

    virtual void Initialize () = 0;
    virtual void Deinitialize () = 0;

    // render
    __EAPI virtual void RenderFrame ();

    // renderer state modifiers
    __EAPI virtual void SetResolution (unsigned int width, unsigned int height);

    // getters
    __EAPI unsigned int GetWidth () const;
    __EAPI unsigned int GetHeight () const;

    // post-processing
    __EAPI virtual void AddPostprocess (Postprocess* postprocess);

    // shader library manipulation
    template <typename T, typename ...Params>
    T& LoadShader (const std::string& shaderName, const std::string& path, ShaderType shaderType);
    
    virtual void AddShaderToProgram (const std::string& shaderProgramName, const std::string& shaderName) = 0;
    virtual ShaderProgram& GetShaderProgram (const std::string& shaderProgramName) = 0;    

    // object drawing and updating
    virtual void Draw (MeshObject& meshObject) = 0;
    virtual void Update (MeshObject& meshObject) = 0;
    virtual void Update (PointLight& pointLight) = 0;
    virtual void Update (DirectionalLight& directionalLight) = 0;
    virtual void Update (SpotLight& spotLight) = 0;
    virtual void Update (Material& material, unsigned int textureUnit) = 0;

protected:

    unsigned int width;
    unsigned int height;

    ResourceManager<Shader> shaderManager;
    ResourceManager<ShaderProgram> shaderProgramManager;

    unsigned int postprocessStage;
    std::vector<Postprocess*> postprocesses;

};

template <typename T, typename ...Params>
T& Renderer::LoadShader (const std::string& shaderName, const std::string& path, ShaderType shaderType)
{
    T& shader = shaderManager.Load<T> (shaderName, path, params...);

    // return shader reference
    return shader;
}

#endif
