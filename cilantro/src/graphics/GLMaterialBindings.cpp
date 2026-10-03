#include "graphics/GLMaterialBindings.h"
#include "scene/Material.h"
#include "resource/Texture.h"
#include <string>

namespace cilantro {

GLMaterialBindings::GLMaterialBindings ()
{
}

GLMaterialBindings::~GLMaterialBindings ()
{
}

void GLMaterialBindings::Update (std::shared_ptr<Material> material, unsigned int textureUnit)
{
    handle_t materialHandle = material->GetHandle ();
    GLuint texture;
    GLuint format;

    auto GLTextureFormat = [](unsigned int numChannels)
    {
        switch (numChannels)
        {
        case 1:
            return GL_RED;
            break;
        case 3:
            return GL_RGB;
            break;
        case 4:
            return GL_RGBA;
            break;
        default:
            return GL_RGB;
        }
    };

    texture_map_t& textures = material->GetTexturesMap ();

    // check if material already exists
    auto find = m_materialTextureUnits.find (materialHandle);

    if (find == m_materialTextureUnits.end ())
    {
        m_materialTextureUnits.insert ({ materialHandle, SGlMaterialTextureUnits () });

        for (auto&& t : textures)
        {
            auto tPtr = t.second.second;
            std::string tName = t.second.first;
            GLuint unit = t.first;
            format = GLTextureFormat (tPtr->GetChannels ());

            glGenTextures (1, &texture);
            glBindTexture (GL_TEXTURE_2D, texture);
            glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
            glTexImage2D (GL_TEXTURE_2D, 0, format, tPtr->GetWidth (), tPtr->GetHeight (), 0, format, GL_UNSIGNED_BYTE, tPtr->Data ());
            glGenerateMipmap (GL_TEXTURE_2D);
            glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture (GL_TEXTURE_2D, 0);

            m_materialTextureUnits[materialHandle].textureUnits[unit] = texture;
        }

        m_materialTextureUnits[materialHandle].unitsCount = (unsigned int) textures.size ();

    }
    else
    {
        auto& t = textures[textureUnit];
        auto tPtr = t.second;
        std::string tName = t.first;
        GLuint unit = textureUnit;
        format = GLTextureFormat (tPtr->GetChannels ());

        glBindTexture (GL_TEXTURE_2D, m_materialTextureUnits[materialHandle].textureUnits[unit]);
        glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D (GL_TEXTURE_2D, 0, format, tPtr->GetWidth (), tPtr->GetHeight (), 0, format, GL_UNSIGNED_BYTE, tPtr->Data ());
        glGenerateMipmap (GL_TEXTURE_2D);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,  GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture (GL_TEXTURE_2D, 0);
    }
}

bool GLMaterialBindings::Bind (std::shared_ptr<Material> material) const
{
    auto find = m_materialTextureUnits.find (material->GetHandle ());

    if (find == m_materialTextureUnits.end ())
    {
        return false;
    }

    const SGlMaterialTextureUnits& u = find->second;

    for (GLuint i = 0; i < u.unitsCount; i++)
    {
        glActiveTexture (GL_TEXTURE0 + i);
        glBindTexture (GL_TEXTURE_2D, u.textureUnits[i]);
    }

    return true;
}

} // namespace cilantro
