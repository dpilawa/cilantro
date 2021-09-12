#ifndef _QUADRENDERSTAGE_H_
#define _QUADRENDERSTAGE_H_

#include "cilantroengine.h"
#include "graphics/ShaderProgram.h"
#include "graphics/RenderStage.h"
#include "math/Vector2f.h"
#include "math/Vector3f.h"
#include "math/Vector4f.h"
#include <string>

class QuadRenderStage : public virtual RenderStage
{
public:
    __EAPI QuadRenderStage ();
    __EAPI virtual ~QuadRenderStage ();

    __EAPI virtual QuadRenderStage& SetShaderProgram (const std::string& shaderProgramName);

    virtual QuadRenderStage& SetRenderStageParameterFloat (const std::string& parameterName, float parameterValue) = 0;
    virtual QuadRenderStage& SetRenderStageParameterVector2f (const std::string& parameterName, const Vector2f& parameterValue) = 0;
    virtual QuadRenderStage& SetRenderStageParameterVector3f (const std::string& parameterName, const Vector3f& parameterValue) = 0;
    virtual QuadRenderStage& SetRenderStageParameterVector4f (const std::string& parameterName, const Vector4f& parameterValue) = 0;

protected:

    ShaderProgram* shaderProgram;

};

#endif