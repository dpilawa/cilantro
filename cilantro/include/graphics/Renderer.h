#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "cilantroengine.h"
#include "resource/ResourceManager.h"
#include "graphics/Framebuffer.h"
#include "graphics/ShaderProgram.h"
#include "graphics/Postprocess.h"
#include <string>

class MeshObject;
class PointLight;
class DirectionalLight;
class SpotLight;
class Material;

class Renderer
{
public:
    __EAPI Renderer (unsigned int width, unsigned int height);
    __EAPI virtual ~Renderer ();

    virtual void Initialize () = 0;
    virtual void Deinitialize () = 0;

    // render
    __EAPI virtual void RenderFrame ();

    // renderbuffer accessor
    __EAPI virtual Framebuffer* GetFramebuffer () const = 0;

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

    Framebuffer* framebuffer;

    unsigned int postprocessStage;

    ResourceManager<ShaderProgram> shaderPrograms;
    ResourceManager<Postprocess> postprocesses;

};

template <typename T, typename ...Params>
T& Renderer::AddPostprocess (const std::string& name, Params&&... params)
{
    T& postprocess = postprocesses.Create<T> (name, params...);

    // initialize
    postprocess.Initialize ();

    // return postprocess
    return postprocess;
}

template <typename T, typename ...Params>
T& Renderer::AddShaderProgram (const std::string& name, Params&&... params)
{
    T& shaderProgram = shaderPrograms.Create<T> (name, params...);

    // return program
    return shaderProgram;
}

#endif
