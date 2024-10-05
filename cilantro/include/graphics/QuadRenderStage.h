#pragma once

#include "cilantroengine.h"
#include "graphics/RenderStage.h"

class CShaderProgram;
class Vector2f;
class Vector3f;
class Vector4f;

class CQuadRenderStage : public CRenderStage
{
public:
    __EAPI CQuadRenderStage ();
    virtual ~CQuadRenderStage () {};

    ///////////////////////////////////////////////////////////////////////////

    virtual void Initialize () override;
    virtual void Deinitialize () override {};

    virtual void OnFrame () override;

    ///////////////////////////////////////////////////////////////////////////

    __EAPI virtual CQuadRenderStage& SetShaderProgram (const std::string& shaderProgramName);

    __EAPI virtual CQuadRenderStage& SetRenderStageParameterFloat (const std::string& parameterName, float parameterValue);
    __EAPI virtual CQuadRenderStage& SetRenderStageParameterVector2f (const std::string& parameterName, const Vector2f& parameterValue);
    __EAPI virtual CQuadRenderStage& SetRenderStageParameterVector3f (const std::string& parameterName, const Vector3f& parameterValue);
    __EAPI virtual CQuadRenderStage& SetRenderStageParameterVector4f (const std::string& parameterName, const Vector4f& parameterValue);

protected:
    virtual void InitializeFramebuffer () override;

protected:

    CShaderProgram* m_shaderProgram;

};
