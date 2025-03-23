#ifndef _PHONGMATERIAL_H_
#define _PHONGMATERIAL_H_

#include "cilantroengine.h"
#include "scene/Material.h"
#include "resource/Texture.h"
#include "math/Vector3f.h"

struct IRenderer;

namespace cilantro {

enum class PhongTexture : int
{
    Diffuse = 0,
    Normal,
    Specular,
    Emissive
};

class __CEAPI PhongMaterial : public Material
{
public:
    __EAPI PhongMaterial (std::shared_ptr<GameScene> scene);
    __EAPI virtual ~PhongMaterial ();

    // setters
    __EAPI PhongMaterial& SetDiffuse (const std::string& diffuse);
    __EAPI PhongMaterial& SetNormal (const std::string& normal);
    __EAPI PhongMaterial& SetSpecular (const std::string& specular);
    __EAPI PhongMaterial& SetEmissive (const std::string& emissive);

    __EAPI PhongMaterial& SetDiffuse (const Vector3f& diffuse);
    __EAPI PhongMaterial& SetSpecular (const Vector3f& specular);
    __EAPI PhongMaterial& SetSpecularShininess (const float specularShininess);
    __EAPI PhongMaterial& SetEmissive (const Vector3f& emissive);


    // getters
    __EAPI std::shared_ptr<Texture> GetDiffuse ();
    __EAPI std::shared_ptr<Texture> GetNormal ();
    __EAPI std::shared_ptr<Texture> GetSpecular ();
    __EAPI float GetSpecularShininess ();
    __EAPI std::shared_ptr<Texture> GetEmissive ();

protected:

    // static textures
    std::shared_ptr<Texture> m_diffuse;
    std::shared_ptr<Texture> m_normal;
    std::shared_ptr<Texture> m_specular;
    std::shared_ptr<Texture> m_emissive;

};

} // namespace cilantro

#endif