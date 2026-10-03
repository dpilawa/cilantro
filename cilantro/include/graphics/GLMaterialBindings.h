#pragma once

#include "cilantroengine.h"
#include "graphics/GLTypes.h"
#include <memory>
#include <unordered_map>

namespace cilantro {

class Material;

// GL textures of materials and their texture units
class GLMaterialBindings
{
public:
    GLMaterialBindings ();
    virtual ~GLMaterialBindings ();

    // create GL textures of a new material or reload texture of an existing material from given texture unit
    void Update (std::shared_ptr<Material> material, unsigned int textureUnit);

    // bind material textures to texture units, returns false if material has no textures loaded
    bool Bind (std::shared_ptr<Material> material) const;

private:
    // texture units of materials (key is material handle)
    std::unordered_map <handle_t, SGlMaterialTextureUnits> m_materialTextureUnits;
};

} // namespace cilantro
