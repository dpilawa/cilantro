#include "graphics/GLLightBuffers.h"
#include "graphics/GLUtils.h"
#include "scene/GameObject.h"
#include "scene/PointLight.h"
#include "scene/DirectionalLight.h"
#include "scene/SpotLight.h"
#include "math/AABB.h"
#include "math/Mathf.h"
#include "math/Vector4f.h"
#include "math/Matrix4f.h"
#include <cmath>
#include <cstring>

namespace cilantro {

GLLightBuffers::GLLightBuffers ()
    : m_pointLightsUBO (0)
    , m_directionalLightsUBO (0)
    , m_spotLightsUBO (0)
    , m_directionalLightViewMatricesUBO (0)
    , m_spotLightViewMatricesUBO (0)
    , m_pointLightViewMatricesUBO (0)
    , m_lightViewMatrixBuffer ()
    , m_pointLightBuffer ()
    , m_directionalLightBuffer ()
    , m_spotLightBuffer ()
{
}

GLLightBuffers::~GLLightBuffers ()
{
}

void GLLightBuffers::Initialize ()
{
    // create unform buffers for light view transforms

    glGenBuffers (1, &m_directionalLightViewMatricesUBO);
    glBindBuffer (GL_UNIFORM_BUFFER, m_directionalLightViewMatricesUBO);
    glBufferData (GL_UNIFORM_BUFFER, 16 * sizeof (GLfloat) * CILANTRO_MAX_DIRECTIONAL_LIGHTS, NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase (GL_UNIFORM_BUFFER, static_cast<int>(EGlUBOType::UBO_DIRECTIONALLIGHTVIEWMATRICES), m_directionalLightViewMatricesUBO);

    glGenBuffers (1, &m_spotLightViewMatricesUBO);
    glBindBuffer (GL_UNIFORM_BUFFER, m_spotLightViewMatricesUBO);
    glBufferData (GL_UNIFORM_BUFFER, 16 * sizeof (GLfloat) * CILANTRO_MAX_SPOT_LIGHTS, NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase (GL_UNIFORM_BUFFER, static_cast<int>(EGlUBOType::UBO_SPOTLIGHTVIEWMATRICES), m_spotLightViewMatricesUBO);

    glGenBuffers (1, &m_pointLightViewMatricesUBO);
    glBindBuffer (GL_UNIFORM_BUFFER, m_pointLightViewMatricesUBO);
    glBufferData (GL_UNIFORM_BUFFER, 6 * 16 * sizeof (GLfloat) * CILANTRO_MAX_POINT_LIGHTS, NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase (GL_UNIFORM_BUFFER, static_cast<int>(EGlUBOType::UBO_POINTLIGHTVIEWMATRICES), m_pointLightViewMatricesUBO);

    GLUtils::CheckGLError (MSG_LOCATION);

    m_pointLightBuffer.pointLightCount = 0;
    m_spotLightBuffer.spotLightCount = 0;
    m_directionalLightBuffer.directionalLightCount = 0;

    // create uniform buffer for point lights
    glGenBuffers (1, &m_pointLightsUBO);
    glBindBuffer (GL_UNIFORM_BUFFER, m_pointLightsUBO);
    glBufferData (GL_UNIFORM_BUFFER, sizeof (SGlUniformPointLightBuffer), &m_pointLightBuffer, GL_DYNAMIC_DRAW);
    glBindBufferBase (GL_UNIFORM_BUFFER, static_cast<int>(EGlUBOType::UBO_POINTLIGHTS), m_pointLightsUBO);

    // create uniform buffer for directional lights
    glGenBuffers (1, &m_directionalLightsUBO);
    glBindBuffer (GL_UNIFORM_BUFFER, m_directionalLightsUBO);
    glBufferData (GL_UNIFORM_BUFFER, sizeof (SGlUniformDirectionalLightBuffer), &m_directionalLightBuffer, GL_DYNAMIC_DRAW);
    glBindBufferBase (GL_UNIFORM_BUFFER, static_cast<int>(EGlUBOType::UBO_DIRECTIONALLIGHTS), m_directionalLightsUBO);

    // create uniform buffer for spot lights
    glGenBuffers (1, &m_spotLightsUBO);
    glBindBuffer (GL_UNIFORM_BUFFER, m_spotLightsUBO);
    glBufferData (GL_UNIFORM_BUFFER, sizeof (SGlUniformSpotLightBuffer), &m_spotLightBuffer, GL_DYNAMIC_DRAW);
    glBindBufferBase (GL_UNIFORM_BUFFER, static_cast<int>(EGlUBOType::UBO_SPOTLIGHTS), m_spotLightsUBO);
}

void GLLightBuffers::Deinitialize ()
{
    glDeleteBuffers (1, &m_directionalLightViewMatricesUBO);
    glDeleteBuffers (1, &m_spotLightViewMatricesUBO);
    glDeleteBuffers (1, &m_pointLightViewMatricesUBO);

    glDeleteBuffers (1, &m_pointLightsUBO);
    glDeleteBuffers (1, &m_directionalLightsUBO);
    glDeleteBuffers (1, &m_spotLightsUBO);
}

bool GLLightBuffers::Update (std::shared_ptr<PointLight> pointLight)
{
    handle_t objectHandle = pointLight->GetHandle ();
    size_t lightId;
    size_t uniformBufferOffset;
    bool isNew = false;

    // check if light is already in collection
    auto find = m_pointLights.find (objectHandle);

    if (find == m_pointLights.end ())
    {
        lightId = m_pointLightBuffer.pointLightCount++;
        m_pointLights.insert ({ objectHandle, lightId });
        isNew = true;
    }
    else
    {
        // existing light modified
        lightId = m_pointLights[objectHandle];
    }

    // copy position
    Vector4f lightPosition = pointLight->GetPosition ();
    m_pointLightBuffer.pointLights[lightId].lightPosition[0] = lightPosition[0];
    m_pointLightBuffer.pointLights[lightId].lightPosition[1] = lightPosition[1];
    m_pointLightBuffer.pointLights[lightId].lightPosition[2] = lightPosition[2];

    // copy attenuation factors
    m_pointLightBuffer.pointLights[lightId].attenuationConst = pointLight->GetConstantAttenuationFactor ();
    m_pointLightBuffer.pointLights[lightId].attenuationLinear = pointLight->GetLinearAttenuationFactor ();
    m_pointLightBuffer.pointLights[lightId].attenuationQuadratic = pointLight->GetQuadraticAttenuationFactor ();

    // copy color
    m_pointLightBuffer.pointLights[lightId].lightColor[0] = pointLight->GetColor ()[0];
    m_pointLightBuffer.pointLights[lightId].lightColor[1] = pointLight->GetColor ()[1];
    m_pointLightBuffer.pointLights[lightId].lightColor[2] = pointLight->GetColor ()[2];

    // copy to GPU memory
    glBindBuffer (GL_UNIFORM_BUFFER, m_pointLightsUBO);

    // load light counts
    glBufferSubData (GL_UNIFORM_BUFFER, 0, sizeof (m_pointLightBuffer.pointLightCount), &m_pointLightBuffer.pointLightCount);

    // load uniform buffer for a light at given index
    uniformBufferOffset = sizeof (m_pointLightBuffer.pointLightCount) + 3 * sizeof (GLint) + lightId * sizeof (SGlPointLightStruct);
    glBufferSubData (GL_UNIFORM_BUFFER, uniformBufferOffset, sizeof (SGlPointLightStruct), &m_pointLightBuffer.pointLights[lightId]);

    glBindBuffer (GL_UNIFORM_BUFFER, 0);

    return isNew;
}

bool GLLightBuffers::Update (std::shared_ptr<DirectionalLight> directionalLight)
{
    handle_t objectHandle = directionalLight->GetHandle ();
    size_t lightId;
    size_t uniformBufferOffset;
    bool isNew = false;

    // check if light is already in collection
    auto find = m_directionalLights.find (objectHandle);

    if (find == m_directionalLights.end ())
    {
        lightId = m_directionalLightBuffer.directionalLightCount++;
        m_directionalLights.insert ({ objectHandle, lightId });
        isNew = true;
    }
    else
    {
        // existing light modified
        lightId = m_directionalLights[objectHandle];
    }

    // copy direction
    Vector3f lightDirection = directionalLight->GetForward ();
    m_directionalLightBuffer.directionalLights[lightId].lightDirection[0] = lightDirection[0];
    m_directionalLightBuffer.directionalLights[lightId].lightDirection[1] = lightDirection[1];
    m_directionalLightBuffer.directionalLights[lightId].lightDirection[2] = lightDirection[2];

    // copy color
    m_directionalLightBuffer.directionalLights[lightId].lightColor[0] = directionalLight->GetColor ()[0];
    m_directionalLightBuffer.directionalLights[lightId].lightColor[1] = directionalLight->GetColor ()[1];
    m_directionalLightBuffer.directionalLights[lightId].lightColor[2] = directionalLight->GetColor ()[2];

    // copy to GPU memory
    glBindBuffer (GL_UNIFORM_BUFFER, m_directionalLightsUBO);

    // load light counts
    glBufferSubData (GL_UNIFORM_BUFFER, 0, sizeof (m_directionalLightBuffer.directionalLightCount), &m_directionalLightBuffer.directionalLightCount);

    // load uniform buffer for a light at given index
    uniformBufferOffset = sizeof (m_directionalLightBuffer.directionalLightCount) + 3 * sizeof (GLint) + lightId * sizeof (SGlDirectionalLightStruct);
    glBufferSubData (GL_UNIFORM_BUFFER, uniformBufferOffset, sizeof (SGlDirectionalLightStruct), &m_directionalLightBuffer.directionalLights[lightId]);

    glBindBuffer (GL_UNIFORM_BUFFER, 0);

    return isNew;
}

bool GLLightBuffers::Update (std::shared_ptr<SpotLight> spotLight)
{
    handle_t objectHandle = spotLight->GetHandle ();
    size_t lightId;
    size_t uniformBufferOffset;
    bool isNew = false;

    // check if light is already in collection
    auto find = m_spotLights.find (objectHandle);

    if (find == m_spotLights.end ())
    {
        lightId = m_spotLightBuffer.spotLightCount++;
        m_spotLights.insert ({ objectHandle, lightId });
        isNew = true;
    }
    else
    {
        // existing light modified
        lightId = m_spotLights[objectHandle];
    }

    // copy position
    Vector4f lightPosition = spotLight->GetPosition ();
    m_spotLightBuffer.spotLights[lightId].lightPosition[0] = lightPosition[0];
    m_spotLightBuffer.spotLights[lightId].lightPosition[1] = lightPosition[1];
    m_spotLightBuffer.spotLights[lightId].lightPosition[2] = lightPosition[2];

    // copy direction
    Vector3f lightDirection = spotLight->GetForward ();
    m_spotLightBuffer.spotLights[lightId].lightDirection[0] = lightDirection[0];
    m_spotLightBuffer.spotLights[lightId].lightDirection[1] = lightDirection[1];
    m_spotLightBuffer.spotLights[lightId].lightDirection[2] = lightDirection[2];

    // copy attenuation factors
    m_spotLightBuffer.spotLights[lightId].attenuationConst = spotLight->GetConstantAttenuationFactor ();
    m_spotLightBuffer.spotLights[lightId].attenuationLinear = spotLight->GetLinearAttenuationFactor ();
    m_spotLightBuffer.spotLights[lightId].attenuationQuadratic = spotLight->GetQuadraticAttenuationFactor ();

    // copy cutoff angles
    m_spotLightBuffer.spotLights[lightId].innerCutoffCosine = std::cos (Mathf::Deg2Rad (spotLight->GetInnerCutoff ()));
    m_spotLightBuffer.spotLights[lightId].outerCutoffCosine = std::cos (Mathf::Deg2Rad (spotLight->GetOuterCutoff ()));

    // copy color
    m_spotLightBuffer.spotLights[lightId].lightColor[0] = spotLight->GetColor ()[0];
    m_spotLightBuffer.spotLights[lightId].lightColor[1] = spotLight->GetColor ()[1];
    m_spotLightBuffer.spotLights[lightId].lightColor[2] = spotLight->GetColor ()[2];

    // copy to GPU memory
    glBindBuffer (GL_UNIFORM_BUFFER, m_spotLightsUBO);

    // load light counts
    glBufferSubData (GL_UNIFORM_BUFFER, 0, sizeof (m_spotLightBuffer.spotLightCount), &m_spotLightBuffer.spotLightCount);

    // load uniform buffer for a light at given index
    uniformBufferOffset = sizeof (m_spotLightBuffer.spotLightCount) + 3 * sizeof (GLint) + lightId * sizeof (SGlSpotLightStruct);
    glBufferSubData (GL_UNIFORM_BUFFER, uniformBufferOffset, sizeof (SGlSpotLightStruct), &m_spotLightBuffer.spotLights[lightId]);

    glBindBuffer (GL_UNIFORM_BUFFER, 0);

    return isNew;
}

void GLLightBuffers::LoadLightViewMatrices (std::shared_ptr<ResourceManager<GameObject>> gameObjects, const std::array<Vector3f, 8>& frustumVertices, const AABB& sceneAABB)
{
    // calculate and load lightview matrix for each directional light
    for (auto&& light : m_directionalLights)
    {
        // generate matrix
        auto l = gameObjects->GetByHandle<DirectionalLight> (light.first);
        Matrix4f lightViewProjection = l->GenLightViewProjectionMatrix (frustumVertices, sceneAABB);

        // copy to buffer
        std::memcpy (m_lightViewMatrixBuffer.directionalLightView + light.second * 16, Mathf::Transpose (lightViewProjection)[0], 16 * sizeof (GLfloat));
    }

    // calculate and load lightview matrix for each spot light
    for (auto&& light : m_spotLights)
    {
        // generate matrix
        auto l = gameObjects->GetByHandle<SpotLight> (light.first);
        Matrix4f lightViewProjection = l->GenLightViewProjectionMatrix (frustumVertices, sceneAABB, false, l->GetOuterCutoff () * 2.0f, l->GetBoundingSphereRadius (0.01f));

        // copy to buffer
        std::memcpy (m_lightViewMatrixBuffer.spotLightView + light.second * 16, Mathf::Transpose (lightViewProjection)[0], 16 * sizeof (GLfloat));
    }

    // calculate and load 6 lightview matrices for each point light
    for (auto&& light : m_pointLights)
    {
        // generate matrices
        auto l = gameObjects->GetByHandle<PointLight> (light.first);
        Vector3f lightPosition = l->GetPosition ();
        Matrix4f lightProjection = Mathf::GenPerspectiveProjectionMatrix (1.0f, Mathf::Deg2Rad (90.0f), l->GetEscapeRadius (), l->GetBoundingSphereRadius (0.01f));

        Matrix4f lightViewRight = Mathf::GenCameraViewMatrix (lightPosition, lightPosition + Vector3f (1.0f, 0.0f, 0.0f), Vector3f (0.0f, -1.0f, 0.0f));
        Matrix4f lightViewLeft = Mathf::GenCameraViewMatrix (lightPosition, lightPosition + Vector3f (-1.0f, 0.0f, 0.0f), Vector3f (0.0f, -1.0f, 0.0f));
        Matrix4f lightViewTop = Mathf::GenCameraViewMatrix (lightPosition, lightPosition + Vector3f (0.0f, 1.0f, 0.0f), Vector3f (0.0f, 0.0f, 1.0f));
        Matrix4f lightViewBottom = Mathf::GenCameraViewMatrix (lightPosition, lightPosition + Vector3f (0.0f, -1.0f, 0.0f), Vector3f (0.0f, 0.0f, -1.0f));
        Matrix4f lightViewFront = Mathf::GenCameraViewMatrix (lightPosition, lightPosition + Vector3f (0.0f, 0.0f, 1.0f), Vector3f (0.0f, -1.0f, 0.0f));
        Matrix4f lightViewBack = Mathf::GenCameraViewMatrix (lightPosition, lightPosition + Vector3f (0.0f, 0.0f, -1.0f), Vector3f (0.0f, -1.0f, 0.0f));

        // copy to buffer
        std::memcpy (m_lightViewMatrixBuffer.pointLightView + light.second * 6 * 16 + 0 * 16, Mathf::Transpose (lightProjection * lightViewRight)[0], 16 * sizeof (GLfloat));
        std::memcpy (m_lightViewMatrixBuffer.pointLightView + light.second * 6 * 16 + 1 * 16, Mathf::Transpose (lightProjection * lightViewLeft)[0], 16 * sizeof (GLfloat));
        std::memcpy (m_lightViewMatrixBuffer.pointLightView + light.second * 6 * 16 + 2 * 16, Mathf::Transpose (lightProjection * lightViewTop)[0], 16 * sizeof (GLfloat));
        std::memcpy (m_lightViewMatrixBuffer.pointLightView + light.second * 6 * 16 + 3 * 16, Mathf::Transpose (lightProjection * lightViewBottom)[0], 16 * sizeof (GLfloat));
        std::memcpy (m_lightViewMatrixBuffer.pointLightView + light.second * 6 * 16 + 4 * 16, Mathf::Transpose (lightProjection * lightViewFront)[0], 16 * sizeof (GLfloat));
        std::memcpy (m_lightViewMatrixBuffer.pointLightView + light.second * 6 * 16 + 5 * 16, Mathf::Transpose (lightProjection * lightViewBack)[0], 16 * sizeof (GLfloat));
    }

    // load to GPU - directional light view
    glBindBuffer (GL_UNIFORM_BUFFER, m_directionalLightViewMatricesUBO);
    glBufferSubData (GL_UNIFORM_BUFFER, 0, 16 * sizeof (GLfloat) * m_directionalLightBuffer.directionalLightCount, m_lightViewMatrixBuffer.directionalLightView);
    glBindBuffer (GL_UNIFORM_BUFFER, 0);

    // load to GPU - spot light view
    glBindBuffer (GL_UNIFORM_BUFFER, m_spotLightViewMatricesUBO);
    glBufferSubData (GL_UNIFORM_BUFFER, 0, 16 * sizeof (GLfloat) * m_spotLightBuffer.spotLightCount, m_lightViewMatrixBuffer.spotLightView);
    glBindBuffer (GL_UNIFORM_BUFFER, 0);

    // load to GPU - point light views
    glBindBuffer (GL_UNIFORM_BUFFER, m_pointLightViewMatricesUBO);
    glBufferSubData (GL_UNIFORM_BUFFER, 0, 6 * 16 * sizeof (GLfloat) * m_pointLightBuffer.pointLightCount, m_lightViewMatrixBuffer.pointLightView);
    glBindBuffer (GL_UNIFORM_BUFFER, 0);
}

size_t GLLightBuffers::GetPointLightCount () const
{
    return m_pointLightBuffer.pointLightCount;
}

size_t GLLightBuffers::GetDirectionalLightCount () const
{
    return m_directionalLightBuffer.directionalLightCount;
}

size_t GLLightBuffers::GetSpotLightCount () const
{
    return m_spotLightBuffer.spotLightCount;
}

} // namespace cilantro
