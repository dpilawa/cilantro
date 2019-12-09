#ifndef _POSTPROCESS_H_
#define _POSTPROCESS_H_

#include "cilantroengine.h"
#include "graphics/ShaderProgram.h"

class PostProcess
{
public:
    PostProcess () = delete;
    __EAPI PostProcess (ShaderProgram* shaderProgram);
    __EAPI virtual ~PostProcess ();

    virtual void OnFrame () = 0;

protected:
    ShaderProgram* shaderProgram;

private:
    virtual void Initialize () = 0;
};

#endif