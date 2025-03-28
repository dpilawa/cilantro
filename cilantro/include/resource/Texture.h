#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "cilantroengine.h"
#include "math/Vector3f.h"
#include "resource/LoadableResource.h"
#include <string>
#include <cstdint>

namespace cilantro {

class __CEAPI Texture : public LoadableResource
{
public:
    __EAPI Texture (const int width, const int height, float channel);
    __EAPI Texture (const int width, const int height, const Vector3f& channels);
    __EAPI Texture (const std::string& path);
    __EAPI virtual ~Texture ();

    __EAPI Texture& GenerateSolid (const int width, const int height, float channel);
    __EAPI Texture& GenerateSolid (const int width, const int height, const Vector3f& channels);

    __EAPI std::uint8_t* Data ();

    __EAPI int GetWidth () const;
    __EAPI int GetHeight () const;
    __EAPI int GetChannels () const;

private:

    __EAPI void Load (const std::string& path) override;

    std::uint8_t* data;
    int width;
    int height;
    int numChannels;

};

} // namespace cilantro

#endif
