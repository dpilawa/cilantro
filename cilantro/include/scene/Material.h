#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "cilantroengine.h"
#include "game/GameComposite.h"
#include "resource/Resource.h"
#include "resource/Texture.h"
#include "math/Vector3f.h"
#include "system/CallbackProvider.h"
#include <vector>
#include <string>
#include <map>
#include <functional>

class Renderer;

typedef std::map<unsigned int, std::pair<std::string, Texture*>> texture_map_t;
typedef std::unordered_map<std::string, std::vector<float>> property_map_t;

class Material : public GameComposite, public Resource, public CallbackProvider<std::string, unsigned int, unsigned int>
{
public:
    __EAPI Material ();
    __EAPI virtual ~Material ();

    __EAPI Material& SetShaderProgram (const std::string& name);
    __EAPI std::string GetShaderProgramName () const;

    texture_map_t& GetTexturesMap();
    property_map_t& GetPropertiesMap ();

protected:

    Material& SetTexture (unsigned int textureUnit, const std::string& label, Texture& texture);
    Material& SetProperty (const std::string& propertyName, float propertyValue);
    Material& SetProperty (const std::string& propertyName, Vector3f propertyValue);

    // textures map
    texture_map_t textures;

    // properties map
    property_map_t properties;

private:

    // name of a shader program defined in renderer
    std::string shaderProgramName;

};

#endif