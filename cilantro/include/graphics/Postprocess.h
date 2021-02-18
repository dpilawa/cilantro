#ifndef _POSTPROCESS_H_
#define _POSTPROCESS_H_

#include "cilantroengine.h"
#include "graphics/Framebuffer.h"
#include "graphics/ShaderProgram.h"
#include <string>

class Renderer;

class Postprocess
{
public:
    __EAPI Postprocess (Renderer& renderer, ShaderProgram& shaderProgram);
    __EAPI virtual ~Postprocess ();

    virtual void Initialize () = 0;
    virtual void Deinitialize () = 0;

    virtual void OnFrame () = 0;

    Framebuffer* GetFramebufferPtr () const;
    virtual void SetPostprocessParameterFloat (const std::string& parameterName, float parameterValue) = 0;

protected:
    Framebuffer* framebuffer;

    Renderer& renderer;
    ShaderProgram& shaderProgram;

};

#endif