#include "cilantroengine.h"
#include "scene/Material.h"
#include "math/Vector3f.h"
#include <string>

Material::Material ()
{
}

Material::~Material ()
{
}

void Material::SetHandle (unsigned int handle)
{
    materialHandle = handle;
}

unsigned int Material::GetHandle () const
{
    return materialHandle;
}

Material& Material::SetShaderProgram (std::string name)
{
    shaderProgramName = name;

    return *this;
}

std::string Material::GetShaderProgramName () const
{
    return shaderProgramName;
}

texture_map_t& Material::GetTexturesMap()
{
    return textures;
}

property_map_t& Material::GetPropertiesMap ()
{
    return properties;
}

Material& Material::SetTexture (unsigned int textureUnit, std::string label, Texture* texture)
{
    textures.emplace (textureUnit, std::make_pair (label, texture));
    InvokeCallbacks ("OnUpdateMaterial", this->materialHandle, textureUnit);

    return *this;
}

Material& Material::SetProperty (std::string propertyName, float propertyValue)
{
    properties[propertyName] = {propertyValue};

    return *this;
}

Material& Material::SetProperty (std::string propertyName, Vector3f propertyValue)
{
    properties[propertyName] = {propertyValue[0], propertyValue[1], propertyValue[2]};

    return *this;
}

   