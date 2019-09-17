#ifndef _RENDERTARGET_H_
#define _RENDERTARGET_H_

#include "GameLoop.h"

class RenderTarget : public GameLoopChild
{
public:
	RenderTarget ();
	virtual ~RenderTarget ();

    __EAPI void SetResolution (unsigned int xRes, unsigned int yRes);

    virtual void Initialize () = 0;
	virtual void Deinitialize () = 0;
	virtual void Bind () = 0;	
	virtual void BeforeFrame () = 0;
	virtual void AfterFrame () = 0;

	unsigned int GetXResolution ();
	unsigned int GetYResolution ();

protected:

	// rendering resolution
	unsigned int xResolution;
	unsigned int yResolution;

};

#endif
