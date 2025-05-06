#include "cilantroengine.h"
#include "math/Mathf.h"
#include "scene/GameScene.h"
#include "scene/GameObject.h"
#include "scene/Light.h"
#include "system/Game.h"

namespace cilantro {

Light::Light (std::shared_ptr<GameScene> gameScene) : GameObject (gameScene)
{
    m_isEnabled = false;
    
    m_lightColor = Vector3f (1.0f, 1.0f, 1.0f);
}

Light::~Light ()
{
}

std::shared_ptr<Light> Light::SetEnabled (bool value)
{
    m_isEnabled = value;
    GetGameScene ()->GetGame ()-> GetMessageBus ()->Publish<LightUpdateMessage> (std::make_shared<LightUpdateMessage> (this->GetHandle ()));
    return std::dynamic_pointer_cast<Light> (shared_from_this ());
}

bool Light::IsEnabled () const
{
    return m_isEnabled;
}

std::shared_ptr<Light> Light::SetColor (Vector3f color)
{
    m_lightColor = color;
    GetGameScene ()->GetGame ()-> GetMessageBus ()->Publish<LightUpdateMessage> (std::make_shared<LightUpdateMessage> (this->GetHandle ()));
    return std::dynamic_pointer_cast<Light> (shared_from_this ());
}

Vector3f Light::GetColor () const
{
    if (m_isEnabled)
    {
        return m_lightColor;
    }
    else
    {
        return Vector3f (0.0f, 0.0f, 0.0f);	
    }
}

void Light::OnUpdate (IRenderer& renderer)
{
    GameObject::OnUpdate (renderer);
}

Matrix4f Light::GenLightViewProjectionMatrix (const std::array<Vector3f, 8>& frustumVertices, const AABB& sceneAABB)
{
    float minX = std::numeric_limits<float>::infinity();
    float maxX = -std::numeric_limits<float>::infinity();
    float minY = std::numeric_limits<float>::infinity();
    float maxY = -std::numeric_limits<float>::infinity();
    float minZ = std::numeric_limits<float>::infinity();
    float maxZ = -std::numeric_limits<float>::infinity();

    Matrix4f lightView = Mathf::GenCameraViewMatrix (Vector3f (), GetForward (), GetUp ());

    // find left, right, top and bottom bounds for the camera frustum in light space
    for (auto&& v : frustumVertices)
    {
        Vector4f lv = lightView * Vector4f (v, 1.0f);

        minX = std::min (minX, lv[0]);
        maxX = std::max (maxX, lv[0]);
        minY = std::min (minY, lv[1]);
        maxY = std::max (maxY, lv[1]);
    }

    // clip AABB in light space to these bounds
    auto aabbTrianglesLightSpace = sceneAABB.GetTriangles (lightView);
    std::vector<Triangle<Vector3f>> aabbTrianglesLightSpaceClip (aabbTrianglesLightSpace.begin (), aabbTrianglesLightSpace.end ());

    Mathf::ClipTrianglesToPlanes (aabbTrianglesLightSpaceClip, Vector3f (minX, minY, -std::numeric_limits<float>::infinity()), Vector3f (maxX, maxY, std::numeric_limits<float>::infinity()));

    // get minZ and maxZ from clipped triangles (this will be near and far planes for the light projection matrix)
    for (auto&& t : aabbTrianglesLightSpaceClip)
    {
        minZ = std::min (minZ, t[0][2]);
        maxZ = std::max (maxZ, t[0][2]);
        minZ = std::min (minZ, t[1][2]);
        maxZ = std::max (maxZ, t[1][2]); 
        minZ = std::min (minZ, t[2][2]);
        maxZ = std::max (maxZ, t[2][2]);
    }

    // finally create the light projection matrix
    return Mathf::GenOrthographicProjectionMatrix (minX, maxX, minY, maxY, minZ, maxZ) * lightView;
}


} // namespace cilantro

