#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "util/CallbackProvider.h"
#include "game/GameLoop.h"
#include "scene/GameScene.h"
#include "scene/MeshObject.h"
#include "scene/PointLight.h"
#include "scene/DirectionalLight.h"
#include "scene/SpotLight.h"
#include "graphics/RenderTarget.h"
#include <string>

enum class ShaderType { VERTEX_SHADER, FRAGMENT_SHADER };

class Renderer : public GameLoopChild
{
public:
	Renderer ();
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

};

#endif
