#ifndef _GLPOSTPROCESS_H_
#define _GLPOSTPROCESS_H_

#include "cilantroengine.h"
#include "graphics/PostProcess.h"
#include "graphics/GLRenderer.h"

class GLPostProcess : public PostProcess
{
public:
    GLPostProcess () = delete;
    __EAPI GLPostProcess (ShaderProgram* shaderProgram);
    __EAPI virtual ~GLPostProcess ();

    virtual void OnFrame ();

private:
    virtual void Initialize ();

    FrameBuffers frameBuffers;
    
};

#endif