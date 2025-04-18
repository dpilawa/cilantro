#pragma once

#include "cilantroengine.h"
#include "graphics/RenderStage.h"

namespace cilantro {

class ShaderProgram;
class Vector2f;
class Vector3f;
class Vector4f;

class __CEAPI SurfaceRenderStage : public RenderStage
{
public:
    __EAPI SurfaceRenderStage (std::shared_ptr<IRenderer> renderer);
    __EAPI virtual ~SurfaceRenderStage ();

    ///////////////////////////////////////////////////////////////////////////

    __EAPI virtual void Initialize () override;
    __EAPI virtual void Deinitialize () override;

    __EAPI virtual void InitializeFramebuffer () override;

    __EAPI virtual void OnFrame () override;

    ///////////////////////////////////////////////////////////////////////////

    __EAPI virtual std::shared_ptr<SurfaceRenderStage> SetShaderProgram (const std::string& shaderProgramName);

    __EAPI virtual std::shared_ptr<SurfaceRenderStage> SetRenderStageParameterFloat (const std::string& parameterName, float parameterValue);
    __EAPI virtual std::shared_ptr<SurfaceRenderStage> SetRenderStageParameterVector2f (const std::string& parameterName, const Vector2f& parameterValue);
    __EAPI virtual std::shared_ptr<SurfaceRenderStage> SetRenderStageParameterVector3f (const std::string& parameterName, const Vector3f& parameterValue);
    __EAPI virtual std::shared_ptr<SurfaceRenderStage> SetRenderStageParameterVector4f (const std::string& parameterName, const Vector4f& parameterValue);

protected:
    std::shared_ptr<ShaderProgram> m_shaderProgram;

};

} // namespace cilantro
