#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "cilantroengine.h"
#include "system/CallbackProvider.h"
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
    virtual void AddShader (std::string shaderName, std::string shaderSourceCode, ShaderType shaderType) = 0;
    virtual void AddShaderToProgram (std::string shaderProgramName, std::string shaderName) = 0;
    virtual ShaderProgram& GetShaderProgram (std::string shaderProgramName) = 0;    

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
    std::vector<Postprocess*> postprocesses;

};

#endif
