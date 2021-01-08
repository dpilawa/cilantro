#include "scene/Texture.h"
#include "util/LogMessage.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture ()
{
    data = nullptr;
}

Texture::~Texture ()
{
    // stbi_image_free (data);
    free (data);
    LogMessage (__func__) << "Unloaded texture" << this;
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

Texture& Texture::Load (const char* filename)
{
    stbi_set_flip_vertically_on_load (true);
    data = stbi_load (filename, &this->width, &this->height, &this->numChannels, 0);

    if (data == nullptr)
    {
        LogMessage (__func__, EXIT_FAILURE) << "Unable to load texture" << filename << "[" << stbi_failure_reason () << "]";
    }
    else 
    {
        LogMessage (__func__) << "Loaded texture" << this << filename << "[" << this->width << this->height << this->numChannels << "]";
    }

    return *this;
}

unsigned char* Texture::Data ()
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