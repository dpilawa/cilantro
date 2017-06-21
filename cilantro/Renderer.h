#ifndef _RENDERER_H_
#define _RENDERER_H_

class MeshObject;
class GameScene;

enum class ShaderType { VERTEX_SHADER, ELEMENT_SHADER };

class Renderer
{
public:
	Renderer (GameScene& scene);
	~Renderer ();

	// abstract functions declarations
	virtual void Initialize () = 0;
	virtual void RenderFrame ();
	virtual void Deinitialize () = 0;

	// shader library manipulation
	virtual void AddShader (std::string shaderName, std::string shaderSourceCode, ShaderType shaderType) = 0;
	virtual void AddShaderModel (std::string shaderModelName, std::string vertexShader, std::string fragmentShader) = 0;

	// object drawing functions
	virtual void Draw (MeshObject& meshobject) = 0;

	// return number of rendered frames
	long long GetFrameCount () const;

protected:
	// GameScene being rendered
	GameScene& renderedScene;

	// number of rendered frames
	long long frameCount;
};

#endif
