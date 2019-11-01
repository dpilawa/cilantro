#include "cilantroengine.h"
#include "graphics/RenderTarget.h"

RenderTarget::RenderTarget ()
{
    xResolution = 256;
    yResolution = 256;
}

RenderTarget::~RenderTarget ()
{
}

void RenderTarget::SetResolution(unsigned int xRes, unsigned int yRes)
{
    xResolution = xRes;
    yResolution = yRes;
}

unsigned int RenderTarget::GetXResolution ()
{
	return xResolution;
}

unsigned int RenderTarget::GetYResolution ()
{
	return yResolution;
}