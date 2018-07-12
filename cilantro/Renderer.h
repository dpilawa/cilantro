#ifndef _RENDERER_H_
#define _RENDERER_H_

enum class ShaderType { VERTEX_SHADER, FRAGMENT_SHADER };

class Renderer
{
public:
	Renderer (GameScene& scene, RenderTarget& target);
	~Renderer ();

	// abstract functions declarations
	virtual void Initialize () = 0;
	virtual void RenderFrame () = 0;
	virtual void Deinitialize () = 0;

	// shader library manipulation
	virtual void AddShader (std::string shaderName, std::string shaderSourceCode, ShaderType shaderType) = 0;
	virtual void AddShaderToModel (std::string shaderModelName, std::string shaderName) = 0;

	// object drawing functions
	virtual void Draw (MeshObject& meshobject) = 0;

protected:

	// GameScene being rendered
	GameScene* renderedScene;

	// RenderTarget in use
	RenderTarget* renderTarget;

};

#endif
