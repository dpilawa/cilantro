#ifndef _GLRENDERSTAGE_H_
#define _GLRENDERSTAGE_H_

#include "cilantroengine.h"
#include "glad/glad.h"
#include "graphics/RenderStage.h"
#include "graphics/GLFramebuffer.h"
#include "graphics/GLFramebuffer.h"
#if (CILANTRO_GL_VERSION > 140)
#include "graphics/GLMultisampleFramebuffer.h"
#endif
#include <string>

class Vector2f;
class Vector3f;
class Vector4f;

class GLRenderStage : public virtual RenderStage
{
public:
    __EAPI GLRenderStage ();
    __EAPI virtual ~GLRenderStage ();

    __EAPI virtual RenderStage& SetMultisampleEnabled (bool value);

    virtual void Initialize ();
    virtual void Deinitialize ();

    virtual void OnFrame ();

};

#endif