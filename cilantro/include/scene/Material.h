#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "cilantroengine.h"
#include "resource/Texture.h"
#include "math/Vector3f.h"
#include "system/CallbackProvider.h"
#include <vector>
#include <string>
#include <map>

class Renderer;

class Material : public CallbackProvider<std::string, unsigned int, unsigned int>
{
public:
    __EAPI Material ();
    __EAPI virtual ~Material ();

    __EAPI void SetHandle (unsigned int handle);
    __EAPI unsigned int GetHandle () const;

    __EAPI virtual void OnUpdate (Renderer &renderer, unsigned int textureUnit = 0) = 0;

    __EAPI Material& SetShaderProgram (std::string name);
    __EAPI std::string GetShaderProgramName () const;

    __EAPI std::map<unsigned int, std::pair<std::string, Texture*>>& GetTexturesMap();
    __EAPI unsigned int GetTexturesMapSize() const;

    __EAPI std::unordered_map<std::string, std::vector<float>>& GetPropertiesMap ();

    __EAPI Material& SetTexture (unsigned int textureUnit, std::string label, Texture* texture);
    __EAPI Material& SetProperty (std::string propertyName, float propertyValue);
    __EAPI Material& SetProperty (std::string propertyName, Vector3f propertyValue);

protected:

    // handle
    unsigned int materialHandle;

    // textures map
    std::map<unsigned int, std::pair<std::string, Texture*>> textures;

    // properties map
    std::unordered_map<std::string, std::vector<float>> properties;

private:

    // name of a shader program defined in renderer
    std::string shaderProgramName;

};

#endif