#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "cilantroengine.h"
#include "scene/GameScene.h"
#include "graphics/ShaderProgram.h"
#include "resource/Resource.h"
#include "resource/Texture.h"
#include "math/Vector3f.h"
#include <vector>
#include <string>
#include <map>
#include <functional>

struct IRenderer;

namespace cilantro {

typedef std::map<unsigned int, std::pair<std::string, std::shared_ptr<Texture>>> texture_map_t;
typedef std::unordered_map<std::string, std::vector<float>> property_map_t;

class __CEAPI Material : public Resource, public std::enable_shared_from_this<Material>
{
public:
    __EAPI Material (std::shared_ptr<GameScene> scene);
    __EAPI virtual ~Material ();

    __EAPI std::shared_ptr<Material> SetForwardShaderProgram (const std::string& name);
    __EAPI std::shared_ptr<Material> SetDeferredGeometryPassShaderProgram (const std::string& name);
    __EAPI std::shared_ptr<Material> SetDeferredLightingPassShaderProgram (const std::string& name);
    __EAPI std::string GetForwardShaderProgram () const;
    __EAPI std::string GetDeferredGeometryPassShaderProgram () const;
    __EAPI std::string GetDeferredLightingPassShaderProgram () const;

    __EAPI std::shared_ptr<GameScene> GetGameScene () const;

    __EAPI texture_map_t& GetTexturesMap();
    __EAPI property_map_t& GetPropertiesMap ();

protected:

    std::shared_ptr<Material> SetTexture (unsigned int textureUnit, const std::string& label, std::shared_ptr<Texture> texture);
    std::shared_ptr<Material> SetProperty (const std::string& propertyName, float propertyValue);
    std::shared_ptr<Material> SetProperty (const std::string& propertyName, Vector3f propertyValue);

    // parent game scene
    std::weak_ptr<GameScene> m_gameScene;

    // textures map
    texture_map_t m_textures;

    // properties map
    property_map_t m_properties;

    // shader programs defined in renderer
    std::string m_forwardShaderProgram;
    std::string m_deferredGeometryPassShaderProgram;
    std::string m_deferredLightingPassShaderProgram;

};

} // namespace cilantro

#endif