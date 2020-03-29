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

Material& Material::SetShaderProgram (std::string name)
{
    shaderProgramName = name;

    return *this;
}

std::string Material::GetShaderProgramName () const
{
    return shaderProgramName;
}

std::unordered_map<std::string, std::vector<float>>& Material::GetPropertiesMap()
{
    return properties;
}