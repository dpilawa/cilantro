#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "cilantroengine.h"
#include "math/Vector3f.h"
#include <vector>
#include <string>
#include <unordered_map>

class Material
{
public:
    __EAPI Material ();
    __EAPI virtual ~Material ();

    __EAPI Material& SetShaderProgram (std::string name);
    __EAPI std::string GetShaderProgramName () const;
    __EAPI std::unordered_map<std::string, std::vector<float>>& GetPropertiesMap();

protected:

    // material properties
    std::unordered_map<std::string, std::vector<float>> properties;

private:

    // name of a shader program defined in renderer
    std::string shaderProgramName;

};

#endif