#pragma once

#include "cilantroengine.h"
#include "graphics/RenderStage.h"

namespace cilantro {

class CShaderProgram;
class Vector2f;
class Vector3f;
class Vector4f;

class QuadRenderStage : public RenderStage
{
public:
    __EAPI QuadRenderStage ();
    virtual ~QuadRenderStage () {};

    ///////////////////////////////////////////////////////////////////////////

    virtual void Initialize () override;
    virtual void Deinitialize () override {};

    virtual void OnFrame () override;

    ///////////////////////////////////////////////////////////////////////////

    __EAPI virtual QuadRenderStage& SetShaderProgram (const std::string& shaderProgramName);

    __EAPI virtual QuadRenderStage& SetRenderStageParameterFloat (const std::string& parameterName, float parameterValue);
    __EAPI virtual QuadRenderStage& SetRenderStageParameterVector2f (const std::string& parameterName, const Vector2f& parameterValue);
    __EAPI virtual QuadRenderStage& SetRenderStageParameterVector3f (const std::string& parameterName, const Vector3f& parameterValue);
    __EAPI virtual QuadRenderStage& SetRenderStageParameterVector4f (const std::string& parameterName, const Vector4f& parameterValue);

protected:
    virtual void InitializeFramebuffer () override;

protected:

    CShaderProgram* m_shaderProgram;

};

} // namespace cilantro
