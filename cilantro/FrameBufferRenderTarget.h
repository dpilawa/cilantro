#ifndef _FRAMEBUFFERRENDERTARGET_H_
#define _FRAMEBUFFERRENDERTARGET_H_

class FrameBufferRenderTarget : public RenderTarget
{
public:
	__EAPI FrameBufferRenderTarget (int xRes, int yRes);
	__EAPI ~FrameBufferRenderTarget ();

	__EAPI void Initialize ();
	__EAPI void Deinitialize ();

	__EAPI void BeforeFrame ();
	__EAPI void AfterFrame ();

private:

	GLuint FBO;
	GLuint RBO;
	GLuint textureColorBuffer;

};

#endif
