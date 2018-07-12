#ifndef _RENDERTARGET_H_
#define _RENDERTARGET_H_

class RenderTarget
{
public:
	RenderTarget (int xRes, int yRes);
	~RenderTarget ();

	virtual void Initialize () = 0;
	virtual void Deinitialize () = 0;
	virtual void BeforeFrame () = 0;
	virtual void AfterFrame () = 0;

	int GetXResolution ();
	int GetYResolution ();

protected:

	// rendering resolution
	int xResolution;
	int yResolution;

};

#endif