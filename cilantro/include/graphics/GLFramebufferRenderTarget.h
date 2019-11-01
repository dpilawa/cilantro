#ifndef _GLFRAMEBUFFERRENDERTARGET_H_
#define _GLFRAMEBUFFERRENDERTARGET_H_

#include "cilantroengine.h"
#include "glad/glad.h"
#include "graphics/RenderTarget.h"

class GLFramebufferRenderTarget : public RenderTarget
{
public:
	__EAPI GLFramebufferRenderTarget ();
	__EAPI ~GLFramebufferRenderTarget ();

	__EAPI void Initialize ();
	__EAPI void Deinitialize ();
	__EAPI void Bind ();
	__EAPI void BeforeFrame ();
	__EAPI void AfterFrame ();

private:

	GLuint FBO;
	GLuint RBO;
	GLuint textureColorBuffer;

};

#endif
