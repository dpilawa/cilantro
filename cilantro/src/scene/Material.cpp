#include "cilantroengine.h"
#include "scene/Material.h"
#include "math/Vector3f.h"
#include <string>

Material::Material () : Resource ()
{
}

Material::~Material ()
{
}

Material& Material::SetShaderProgram (const std::string& name)
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

Material& Material::SetTexture (unsigned int textureUnit, const std::string& label, Texture& texture)
{
    textures[textureUnit] = std::pair (label, &texture);
    InvokeCallbacks ("OnUpdateMaterial", this->GetHandle (), textureUnit);

    return *this;
}

Material& Material::SetProperty (const std::string& propertyName, float propertyValue)
{
    properties[propertyName] = {propertyValue};

    return *this;
}

Material& Material::SetProperty (const std::string& propertyName, Vector3f propertyValue)
{
    properties[propertyName] = {propertyValue[0], propertyValue[1], propertyValue[2]};

    return *this;
}

   