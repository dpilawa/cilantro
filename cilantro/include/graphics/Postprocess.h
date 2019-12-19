#ifndef _POSTPROCESS_H_
#define _POSTPROCESS_H_

#include "cilantroengine.h"
#include "graphics/ShaderProgram.h"
#include <string>

class Renderer;

class Postprocess
{
public:
    Postprocess () = delete;
    __EAPI Postprocess (Renderer* renderer, ShaderProgram* shaderProgram);
    __EAPI virtual ~Postprocess ();

    virtual void OnFrame () = 0;

    virtual void SetPostprocessParameterFloat (std::string parameterName, float parameterValue) = 0;

protected:
    Renderer* renderer;
    ShaderProgram* shaderProgram;

};

#endif