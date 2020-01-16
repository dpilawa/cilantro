#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "cilantroengine.h"
#include "util/CallbackProvider.h"
#include "game/GameLoop.h"
#include "scene/GameScene.h"
#include "scene/MeshObject.h"
#include "scene/PointLight.h"
#include "scene/DirectionalLight.h"
#include "scene/SpotLight.h"
#include "graphics/Shader.h"
#include "graphics/RenderTarget.h"
#include "graphics/Postprocess.h"
#include <string>

class Renderer
{
public:
	Renderer () = delete;
	__EAPI Renderer (GameLoop* gameLoop, unsigned int width, unsigned int height);
	__EAPI virtual ~Renderer ();

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

	// object drawing and updating
	virtual void Draw (MeshObject& meshObject) = 0;
	virtual void Update (MeshObject& meshObject) = 0;
	virtual void Update (PointLight& pointLight) = 0;
	virtual void Update (DirectionalLight& pointLight) = 0;
	virtual void Update (SpotLight& pointLight) = 0;

protected:

	GameLoop* gameLoop;

	unsigned int width;
	unsigned int height;

    unsigned int postprocessStage;
    std::vector<Postprocess*> postprocesses;

    virtual void Initialize () = 0;
	virtual void Deinitialize () = 0;
};

#endif
