#include "cilantroengine.h"
#include "scene/Material.h"
#include "math/Vector3f.h"
#include "system/EngineContext.h"
#include <string>

Material::Material () : Resource ()
{
}

Material::~Material ()
{
}

Material& Material::SetForwardShaderProgram (const std::string& name)
{
    forwardShaderProgram = EngineContext::GetRenderer ().GetShaderProgramManager ().GetByName<ShaderProgram> (name).GetHandle ();
    InvokeCallbacks ("OnUpdateMaterial", this->GetHandle (), 0);

    return *this;
}

Material& Material::SetDeferredGeometryPassShaderProgram (const std::string& name)
{
    deferredGeometryPassShaderProgram = EngineContext::GetRenderer ().GetShaderProgramManager ().GetByName<ShaderProgram> (name).GetHandle ();
    InvokeCallbacks ("OnUpdateMaterial", this->GetHandle (), 0);

    return *this;
}

Material& Material::SetDeferredLightingPassShaderProgram (const std::string& name)
{
    deferredLightingPassShaderProgram = EngineContext::GetRenderer ().GetShaderProgramManager ().GetByName<ShaderProgram> (name).GetHandle ();
    InvokeCallbacks ("OnUpdateMaterial", this->GetHandle (), 0);

    return *this;
}

ShaderProgram& Material::GetForwardShaderProgram () const
{
    return EngineContext::GetRenderer ().GetShaderProgramManager ().GetByHandle<ShaderProgram> (forwardShaderProgram);
}

ShaderProgram& Material::GetDeferredGeometryPassShaderProgram () const
{
    return EngineContext::GetRenderer ().GetShaderProgramManager ().GetByHandle<ShaderProgram> (deferredGeometryPassShaderProgram);
}

ShaderProgram& Material::GetDeferredLightingPassShaderProgram () const
{
    return EngineContext::GetRenderer ().GetShaderProgramManager ().GetByHandle<ShaderProgram> (deferredLightingPassShaderProgram);
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

   