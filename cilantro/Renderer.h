#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "CallbackProvider.h"
#include "GameScene.h"
#include "MeshObject.h"
#include "PointLight.h"
#include "DirectionalLight.h"
#include "SpotLight.h"
#include "RenderTarget.h"
#include <string>

enum class ShaderType { VERTEX_SHADER, FRAGMENT_SHADER };

class Renderer
{
public:
	Renderer (GameScene& scene, RenderTarget& target);
	virtual ~Renderer ();

	// abstract functions declarations
	virtual void Initialize () = 0;
	virtual void RenderFrame () = 0;
	virtual void Deinitialize () = 0;

	// shader library manipulation
	virtual void AddShader (std::string shaderName, std::string shaderSourceCode, ShaderType shaderType) = 0;
	virtual void AddShaderToModel (std::string shaderModelName, std::string shaderName) = 0;

	// object drawing and updating
	virtual void Draw (MeshObject& meshObject) = 0;
	virtual void Update (MeshObject& meshObject) = 0;
	virtual void Update (PointLight& pointLight) = 0;
	virtual void Update (DirectionalLight& pointLight) = 0;
	virtual void Update (SpotLight& pointLight) = 0;

protected:

	// GameScene being rendered
	GameScene* renderedScene;

	// RenderTarget in use
	RenderTarget* renderTarget;

};

#endif
