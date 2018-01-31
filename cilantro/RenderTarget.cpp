#include "RenderTarget.h"

RenderTarget::RenderTarget (int xRes, int yRes) : xResolution (xRes), yResolution (yRes)
{
}

RenderTarget::~RenderTarget ()
{
}

int RenderTarget::GetXResolution ()
{
	return xResolution;
}

int RenderTarget::GetYResolution ()
{
	return yResolution;
}