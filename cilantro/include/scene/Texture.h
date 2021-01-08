#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "cilantroengine.h"
#include "math/Vector3f.h"

class Texture
{
public:
    __EAPI Texture ();
    __EAPI virtual ~Texture ();

    __EAPI Texture& GenerateSolid (const int width, const int height, float channel);
    __EAPI Texture& GenerateSolid (const int width, const int height, const Vector3f& channels);
    __EAPI Texture& Load (const char *filename);

    __EAPI unsigned char* Data ();

    __EAPI int GetWidth () const;
    __EAPI int GetHeight () const;
    __EAPI int GetChannels () const;

private:

    unsigned char* data;
    int width;
    int height;
    int numChannels;

};

#endif