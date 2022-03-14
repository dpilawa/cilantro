#include "cilantroengine.h"
#include "scene/Material.h"
#include "math/Vector3f.h"
#include "system/Game.h"
#include <string>

Material::Material () : CResource ()
{
}

Material::~Material ()
{
}

Material& Material::SetForwardShaderProgram (const std::string& name)
{
    forwardShaderProgram = name;
    InvokeCallbacks ("OnUpdateMaterial", this->GetHandle (), 0);

    return *this;
}

Material& Material::SetDeferredGeometryPassShaderProgram (const std::string& name)
{
    deferredGeometryPassShaderProgram = name;
    InvokeCallbacks ("OnUpdateMaterial", this->GetHandle (), 0);

    return *this;
}

Material& Material::SetDeferredLightingPassShaderProgram (const std::string& name)
{
    deferredLightingPassShaderProgram = name;
    InvokeCallbacks ("OnUpdateMaterial", this->GetHandle (), 0);

    return *this;
}

std::string Material::GetForwardShaderProgram () const
{
    return forwardShaderProgram;
}

std::string Material::GetDeferredGeometryPassShaderProgram () const
{
    return deferredGeometryPassShaderProgram;
}

std::string Material::GetDeferredLightingPassShaderProgram () const
{
    return deferredLightingPassShaderProgram;
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
    InvokeCallbacks ("OnUpdateMaterialTexture", this->GetHandle (), textureUnit);

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

   