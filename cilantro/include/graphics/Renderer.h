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
#include "graphics/ShaderProgram.h"
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

    // access to game resource manager
    __EAPI virtual ResourceManager<Resource>& GetGameResourceManager ();

    // post-processing
    __EAPI virtual ResourceManager<Postprocess>& GetPostprocessManager ();
    
    template <typename T, typename ...Params>
    T& AddPostprocess (const std::string& name, Params&&... params);

    // shader library manipulation
    __EAPI virtual ResourceManager<ShaderProgram>& GetShaderProgramManager ();
    
    template <typename T, typename ...Params>
    T& AddShaderProgram (const std::string& name, Params&&... params);        

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

    unsigned int postprocessStage;

    ResourceManager<ShaderProgram> shaderPrograms;
    ResourceManager<Postprocess> postprocesses;

};

template <typename T, typename ...Params>
T& Renderer::AddPostprocess (const std::string& name, Params&&... params)
{
    T& postprocess = postprocesses.Create<T> (name, params...);

    // set renderer and initialize
    postprocess.AttachToRenderer (this);
    postprocess.Initialize ();

    // return postprocess
    return postprocess;
}

template <typename T, typename ...Params>
T& Renderer::AddShaderProgram (const std::string& name, Params&&... params)
{
    T& shaderProgram = shaderPrograms.Create<T> (name, params...);

    // set renderer and initialize
    shaderProgram.AttachToRenderer (this);

    // return program
    return shaderProgram;
}

#endif
