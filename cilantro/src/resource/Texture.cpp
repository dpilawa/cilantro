#include "resource/Texture.h"
#include "system/LogMessage.h"
#include "system/Game.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace cilantro {

Texture::Texture (const int width, const int height, float channel) : LoadableResource ()
{
    data = nullptr;
    GenerateSolid (width, height, channel);
}

Texture::Texture (const int width, const int height, const Vector3f& channels) : LoadableResource ()
{
    data = nullptr;
    GenerateSolid (width, height, channels);
}

Texture::Texture (const std::string& path) : LoadableResource (path)
{
    Load (path.c_str ());
}

Texture::~Texture ()
{    
    free (data);
}

Texture& Texture::GenerateSolid (const int width, const int height, float channel)
{
    this->width = width;
    this->height = height;
    this->numChannels = 1;

    if (data != nullptr)
    {
        free (data);
    }

    data = (unsigned char*) malloc (this->width * this->height * this->numChannels * sizeof (float));

    for (int i = 0; i < width * height; i += numChannels)
    {
        data[i] = (char) (channel * 255.0f);
    }

    return *this;
}

Texture& Texture::GenerateSolid (const int width, const int height, const Vector3f& channels)
{
    this->width = width;
    this->height = height;
    this->numChannels = 3;

    if (data != nullptr)
    {
        free (data);
    }

    data = (unsigned char*) malloc (this->width * this->height * this->numChannels * sizeof (float));

    for (int i = 0; i < width * height * numChannels; i += numChannels)
    {
        data[i] = (char) (channels[0] * 255.0f);
        data[i + 1] = (char) (channels[1] * 255.0f);
        data[i + 2] = (char) (channels[2] * 255.0f);
    }

    return *this;
}

std::uint8_t* Texture::Data ()
{
    return this->data;
}

int Texture::GetWidth () const
{
    return this->width;
}

int Texture::GetHeight () const
{
    return this->height;
}

int Texture::GetChannels () const
{
    return this->numChannels;
}

void Texture::Load (const std::string& path)
{
    stbi_set_flip_vertically_on_load (true);
    data = stbi_load (path.c_str (), &this->width, &this->height, &this->numChannels, STBI_default);

    if (data == nullptr)
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Unable to load texture" << path << "[" << stbi_failure_reason () << "]";
    }
}

} // namespace cilantro

