#ifndef _GLPOSTPROCESS_H_
#define _GLPOSTPROCESS_H_

#include "cilantroengine.h"
#include "graphics/PostProcess.h"
#include "graphics/GLRenderer.h"

class GLPostProcess : public PostProcess
{
public:
    __EAPI GLPostProcess ();
    __EAPI virtual ~GLPostProcess ();
private:

    FrameBuffers frameBuffers;
};

#endif