/* light data structures */

struct PointLightStruct
{
    vec3 lightPosition;	/* world space */
    vec3 lightColor;
    float attenuationConst;
    float attenuationLinear;
    float attenuationQuadratic;
};

struct DirectionalLightStruct
{
    vec3 lightDirection;
    vec3 lightColor;
};

struct SpotLightStruct
{
    vec3 lightPosition;	/* world space */		
    vec3 lightDirection;
    vec3 lightColor;
    float attenuationConst;
    float attenuationLinear;
    float attenuationQuadratic;		
    float innerCutoffCosine;
    float outerCutoffCosine;
};

#if (__VERSION__ >= 420)
layout(std140, binding = %%UBO_POINTLIGHTS%%) uniform UniformPointLightsBlock
{
    int pointLightCount;
    PointLightStruct pointLights[%%CILANTRO_MAX_POINT_LIGHTS%%];
};
#else
layout(std140) uniform UniformPointLightsBlock
{
    int pointLightCount;
    PointLightStruct pointLights[%%CILANTRO_MAX_POINT_LIGHTS%%];
};
#endif

#if (__VERSION__ >= 420)
layout(std140, binding = %%UBO_DIRECTIONALLIGHTS%%) uniform UniformDirectionalLightsBlock
{
    int directionalLightCount;
    DirectionalLightStruct directionalLights[%%CILANTRO_MAX_DIRECTIONAL_LIGHTS%%];
};
#else
layout(std140) uniform UniformDirectionalLightsBlock
{
    int directionalLightCount;
    DirectionalLightStruct directionalLights[%%CILANTRO_MAX_DIRECTIONAL_LIGHTS%%];
};
#endif

#if (__VERSION__ >= 420)
layout(std140, binding = %%UBO_SPOTLIGHTS%%) uniform UniformSpotLightsBlock
{
    int spotLightCount;
    SpotLightStruct spotLights[%%CILANTRO_MAX_SPOT_LIGHTS%%];
};
#else
layout(std140) uniform UniformSpotLightsBlock
{
    int spotLightCount;
    SpotLightStruct spotLights[%%CILANTRO_MAX_SPOT_LIGHTS%%];
};
#endif