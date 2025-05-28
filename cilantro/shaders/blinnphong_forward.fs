#version %%CILANTRO_GLSL_VERSION%%

#define BIAS %%CILANTRO_SHADOW_BIAS%%
#define MAX_POINT_LIGHTS %%CILANTRO_MAX_POINT_LIGHTS%%
#define MAX_DIRECTIONAL_LIGHTS %%CILANTRO_MAX_DIRECTIONAL_LIGHTS%%
#define MAX_SPOT_LIGHTS %%CILANTRO_MAX_SPOT_LIGHTS%%

const int PCFPOWER = 1;

/* fragment position in world space */
in vec3 fPosition;

/* TBN matrix */
in mat3 TBN;

/* texture uv coordinates */
in vec2 fUV;

/* viewing direction */
vec3 viewDirection;

/* material properties */
#if (__VERSION__ >= 430)
layout (binding = 0) uniform sampler2D tDiffuse;
layout (binding = 1) uniform sampler2D tNormal;
layout (binding = 2) uniform sampler2D tSpecular;
layout (binding = 3) uniform sampler2D tEmissive;
#else
uniform sampler2D tDiffuse;
uniform sampler2D tNormal;
uniform sampler2D tSpecular;
uniform sampler2D tEmissive;
#endif

/* shadow maps */
#if (__VERSION__ >= 430)
layout (binding = 4) uniform sampler2DArray tShadowMap;
#else
uniform sampler2DArray tShadowMap;
#endif

vec3 fNormal;
vec3 fDiffuseColor;
vec3 fSpecularColor;
vec3 fEmissiveColor;
uniform float fSpecularShininess;

/* eye position in world space */
uniform vec3 eyePosition;

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

layout(std140, binding = %%UBO_POINTLIGHTS%%) uniform UniformPointLightsBlock
{
    int pointLightCount;
    PointLightStruct pointLights[MAX_POINT_LIGHTS];
};

layout(std140, binding = %%UBO_DIRECTIONALLIGHTS%%) uniform UniformDirectionalLightsBlock
{
    int directionalLightCount;
    DirectionalLightStruct directionalLights[MAX_DIRECTIONAL_LIGHTS];
};

layout(std140, binding = %%UBO_SPOTLIGHTS%%) uniform UniformSpotLightsBlock
{
    int spotLightCount;
    SpotLightStruct spotLights[MAX_SPOT_LIGHTS];
};

layout (std140, binding = %%UBO_DIRECTIONALLIGHTVIEWMATRICES%%) uniform UniformDirectionalLightViewMatricesBlock
{
    mat4 mDirectionalLightSpace[MAX_DIRECTIONAL_LIGHTS];
};

layout (std140, binding = %%UBO_SPOTLIGHTVIEWMATRICES%%) uniform UniformSpotLightViewMatricesBlock
{
    mat4 mSpotLightSpace[MAX_SPOT_LIGHTS];
};

/* output color */
out vec4 color;

/* calculate diffuse coefficient */
float CalculateDiffuse (float n_dot_l)
{
    return clamp (n_dot_l, 0.0, 1.0);
}

/* calculate specular coefficient */
float CalculateSpecular (vec3 lightDirection, float n_dot_l)
{
    vec3 halfwayDirection = normalize (lightDirection + viewDirection);
    return pow (clamp (dot (fNormal, halfwayDirection), 0.0, 1.0), fSpecularShininess) * smoothstep(0.0, 0.5, n_dot_l);		
}

vec3 CalculateOutputColor (float diffuseCoefficient, float attenuationFactor, float specularCoefficient, float shadow, vec3 lightColor)
{
    return (diffuseCoefficient * fDiffuseColor * shadow * attenuationFactor + specularCoefficient * fSpecularColor) * lightColor;
}

/* calculate contribution of one point light */
vec4 CalculatePointLight (PointLightStruct light, int pointLightIdx)
{
    vec3 lightDirection = normalize (light.lightPosition - fPosition);

    /* diffuse component */
    float n_dot_l = dot (fNormal, lightDirection);
    float diffuseCoefficient = CalculateDiffuse (n_dot_l);
    
    /* specular component */
    float specularCoefficient = CalculateSpecular (lightDirection, n_dot_l);
    
    /* attenuation */
    float distanceToLight = length (light.lightPosition - fPosition);
    float attenuationFactor = 1.0f / (light.attenuationConst + light.attenuationLinear * distanceToLight + light.attenuationQuadratic * distanceToLight * distanceToLight);

    /* aggregate output */
    return vec4 (CalculateOutputColor (diffuseCoefficient, attenuationFactor, specularCoefficient, 1.0, light.lightColor), 1.0);
}

/* calculate directional light shadow */
float CalculateDirectionalLightShadow (int directionalLightIdx)
{
    vec4 fragmentLightSpace = mDirectionalLightSpace[directionalLightIdx] * vec4 (fPosition, 1.0);
    vec3 depthMapCoords = vec3 (fragmentLightSpace / fragmentLightSpace.w) * 0.5 + 0.5;
    float fragmentDepth = depthMapCoords.z;

    if (depthMapCoords.x < 0.0 || depthMapCoords.x > 1.0 ||
        depthMapCoords.y < 0.0 || depthMapCoords.y > 1.0 ||
        depthMapCoords.z < 0.0 || depthMapCoords.z > 1.0)
        return 1.0;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(tShadowMap, 0).xy;
    
    for(int x = -PCFPOWER; x <= PCFPOWER; ++x)
    {
        for(int y = -PCFPOWER; y <= PCFPOWER; ++y)
        {
            float pcfDepth = texture(tShadowMap, vec3 (depthMapCoords.xy + vec2 (x, y) * texelSize, directionalLightIdx)).r; 
            shadow += (fragmentDepth - BIAS < pcfDepth) ? 1.0 : 0.0;        
        }    
    }
    shadow /= pow (PCFPOWER * 2 + 1, 2);

    return shadow;
}

/* calculate contribution of one directional light */
vec4 CalculateDirectionalLight (DirectionalLightStruct light, int directionalLightIdx)
{
    vec3 lightDirection = normalize (-light.lightDirection);

    /* diffuse component */
    float n_dot_l = dot (fNormal, lightDirection);		
    float diffuseCoefficient = CalculateDiffuse (n_dot_l);

    /* shadow */
    float shadow = CalculateDirectionalLightShadow (directionalLightIdx);

    /* specular component */
    float specularCoefficient = CalculateSpecular (lightDirection, n_dot_l);

    /* attenuation */
    float attenuationFactor = 1.0f;

    /* aggregate output */
    return vec4 (CalculateOutputColor (diffuseCoefficient, attenuationFactor, specularCoefficient, shadow, light.lightColor), 1.0);
}

/* calculate spot light shadow */
float CalculateSpotLightShadow (int spotLightIdx)
{
    vec4 fragmentLightSpace = mSpotLightSpace[spotLightIdx] * vec4 (fPosition, 1.0);
    vec3 depthMapCoords = vec3 (fragmentLightSpace / fragmentLightSpace.w) * 0.5 + 0.5;
    float fragmentDepth = depthMapCoords.z;

    if (depthMapCoords.x < 0.0 || depthMapCoords.x > 1.0 ||
        depthMapCoords.y < 0.0 || depthMapCoords.y > 1.0 ||
        depthMapCoords.z < 0.0 || depthMapCoords.z > 1.0)
        return 1.0;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(tShadowMap, 0).xy;

    for (int x = -PCFPOWER; x <= PCFPOWER; ++x)
    {
        for (int y = -PCFPOWER; y <= PCFPOWER; ++y)
        {
            vec2 offset = vec2(x, y) * texelSize;
            float pcfDepth = texture(tShadowMap, vec3(depthMapCoords.xy + offset, directionalLightCount + spotLightIdx)).r;
            shadow += ((fragmentDepth - BIAS) < pcfDepth) ? 1.0 : 0.0;
        }
    }
    shadow /= pow (PCFPOWER * 2 + 1, 2);

    return shadow;
}

/* calculate contribution of one spot light */
vec4 CalculateSpotLight (SpotLightStruct light, int spotLightIdx)
{
    vec3 lightDirection = normalize (light.lightPosition - fPosition);

    /* check if in cone */
    float theta = dot (lightDirection, normalize (-light.lightDirection));
    float epsilon = light.innerCutoffCosine - light.outerCutoffCosine;
    float intensity = clamp ((theta - light.outerCutoffCosine) / epsilon, 0.0, 1.0); 		

    /* shadow */
    float shadow = CalculateSpotLightShadow (spotLightIdx);

    /* diffuse component */
    float n_dot_l = dot (fNormal, lightDirection);
    float diffuseCoefficient = CalculateDiffuse (n_dot_l);
    
    /* specular component */
    float specularCoefficient = CalculateSpecular (lightDirection, n_dot_l);
    
    /* attenuation */
    float distanceToLight = length (light.lightPosition - fPosition);
    float attenuationFactor = 1.0f / (light.attenuationConst + light.attenuationLinear * distanceToLight + light.attenuationQuadratic * distanceToLight * distanceToLight);
        
    /* aggregate output */
    return vec4 (CalculateOutputColor (diffuseCoefficient, attenuationFactor, specularCoefficient, shadow, light.lightColor) * intensity, 1.0);
}

void main()
{
    fDiffuseColor = texture (tDiffuse, fUV).rgb;
    fSpecularColor = texture (tSpecular, fUV).rgb;
    fEmissiveColor = texture (tEmissive, fUV).rgb;
    fNormal = normalize (TBN * (texture (tNormal, fUV).rgb * 2.0 - 1.0));
    
    color = vec4 (fEmissiveColor, 1.0);
    
    /* calculate viewing direction */
    viewDirection = normalize (eyePosition - fPosition);

    for (int i=0; i < pointLightCount; i++)
    {
        color += CalculatePointLight (pointLights[i], i);
    }

    for (int i=0; i < directionalLightCount; i++)
    {
        color += CalculateDirectionalLight (directionalLights[i], i);
    }

    for (int i=0; i < spotLightCount; i++)
    {
        color += CalculateSpotLight (spotLights[i], i);
    }		
} 