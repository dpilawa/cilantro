#version %%CILANTRO_GLSL_VERSION%%

#define MAX_POINT_LIGHTS %%CILANTRO_MAX_POINT_LIGHTS%%
#define MAX_DIRECTIONAL_LIGHTS %%CILANTRO_MAX_DIRECTIONAL_LIGHTS%%
#define MAX_SPOT_LIGHTS %%CILANTRO_MAX_SPOT_LIGHTS%%

const float PI = 3.14159265359;
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
layout (binding = 0) uniform sampler2D tAlbedo;
layout (binding = 1) uniform sampler2D tNormal;
layout (binding = 2) uniform sampler2D tMetallic;
layout (binding = 3) uniform sampler2D tRoughness;
layout (binding = 4) uniform sampler2D tAO;
#else
uniform sampler2D tAlbedo;
uniform sampler2D tNormal;
uniform sampler2D tMetallic;
uniform sampler2D tRoughness;
uniform sampler2D tAO;
#endif

/* shadow maps */
#if (__VERSION__ >= 430)
layout (binding = 5) uniform sampler2DArray tDirectionalShadowMap;
#else
uniform sampler2DArray tDirectionalShadowMap;
#endif

vec3 fNormal;
vec3 fAlbedo;
float fMetallic;
float fRoughness;
float fAO;

/* eye position in world space */
uniform vec3 eyePosition;

/* point lights data structures */

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
    highp mat4 mDirectionalLightSpace[MAX_DIRECTIONAL_LIGHTS];
};

/* output color */
out vec4 color;

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow (1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max (dot (N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max (dot (N, V), 0.0);
    float NdotL = max (dot (N, L), 0.0);
    float ggx2 = GeometrySchlickGGX (NdotV, roughness);
    float ggx1 = GeometrySchlickGGX (NdotL, roughness);
    
    return ggx1 * ggx2;
}

vec3 CalculateCookTorranceSpecularBRDF (vec3 N, vec3 L, vec3 V, vec3 H)
{
    vec3 F0 = vec3 (0.04); 
    F0 = mix (F0, fAlbedo, fMetallic);
    vec3 F = fresnelSchlick (max (dot (H, V), 0.0), F0);

    float NDF = DistributionGGX (N, H, fRoughness);       
    float G = GeometrySmith (N, V, L, fRoughness);       

    return (NDF * G * F) / max (4.0 * max (dot (N, V), 0.0) * max (dot (N, L), 0.0), 0.001);
}

/* calculate point light radiance */
vec3 CalculatePointLightRadiance (PointLightStruct light)
{
    /* attenuation */
    float distanceToLight = length (light.lightPosition - fPosition);
    float attenuationFactor = 1.0f / (light.attenuationConst + light.attenuationLinear * distanceToLight + light.attenuationQuadratic * distanceToLight * distanceToLight);

    /* radiance */
    return attenuationFactor * light.lightColor;
}

/* calculate directional light shadow */
float CalculateDirectionalLightShadow (int directionalLightIdx)
{
    vec4 fragmentLightSpace = mDirectionalLightSpace[directionalLightIdx] * vec4 (fPosition, 1.0);
    vec3 depthMapCoords = vec3 (fragmentLightSpace / fragmentLightSpace.w) * 0.5 + 0.5;
    float fragmentDepth = abs (depthMapCoords.z);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(tDirectionalShadowMap, 0).xy;
    
    for(int x = -PCFPOWER; x <= PCFPOWER; ++x)
    {
        for(int y = -PCFPOWER; y <= PCFPOWER; ++y)
        {
            float pcfDepth = texture(tDirectionalShadowMap, vec3 (depthMapCoords.xy + vec2 (x, y) * texelSize, directionalLightIdx)).r; 
            shadow += (fragmentDepth < pcfDepth || (fragmentDepth > 1.0)) ? 1.0 : 0.0;        
        }    
    }
    shadow /= pow (PCFPOWER * 2 + 1, 2);

    return shadow;
}

/* calculate directional light radiance */
vec3 CalculateDirectionalLightRadiance (DirectionalLightStruct light)
{
    /* radiance (directional light has no attenuation) */
    return light.lightColor;
}

/* calculate spot light radiance */
vec3 CalculateSpotLightRadiance (SpotLightStruct light, vec3 L)
{
    /* check if in cone */
    float theta = dot (L, normalize (-light.lightDirection));
    float epsilon = light.innerCutoffCosine - light.outerCutoffCosine;
    float intensity = clamp ((theta - light.outerCutoffCosine) / epsilon, 0.0, 1.0); 		
    
    /* attenuation */
    float distanceToLight = length (light.lightPosition - fPosition);
    float attenuationFactor = 1.0f / (light.attenuationConst + light.attenuationLinear * distanceToLight + light.attenuationQuadratic * distanceToLight * distanceToLight);
        
    /* radiance */
    return intensity * attenuationFactor * light.lightColor;
}

void main()
{
    vec3 Lo = vec3 (0.0);
    
    /* sample normal map */
    fNormal = normalize (TBN * (texture (tNormal, fUV).rgb * 2.0 - 1.0));

    /* calculate viewing direction */
    viewDirection = normalize (eyePosition - fPosition);

    /* sample textures */
    fAlbedo = texture (tAlbedo, fUV).rgb;
    fMetallic = texture (tMetallic, fUV).r;
    fRoughness = texture (tRoughness, fUV).r;
    fAO = texture (tAO, fUV).r;

    for (int i = 0; i < pointLightCount; i++)
    {
        vec3 lightDirection = normalize (pointLights[i].lightPosition - fPosition);
        vec3 halfwayDirection = normalize (viewDirection + lightDirection);

        vec3 radiance = CalculatePointLightRadiance (pointLights[i]);
        vec3 specular = CalculateCookTorranceSpecularBRDF (fNormal, lightDirection, viewDirection, halfwayDirection);
        
        vec3 F0 = vec3(0.04); 
        F0 = mix(F0, fAlbedo, fMetallic);
        vec3 F = fresnelSchlick (max (dot (halfwayDirection, viewDirection), 0.0), F0);

        vec3 kD = vec3 (1.0) - F;
        kD = kD * (1.0 - fMetallic);	

        float NdotL = max (dot (fNormal, lightDirection), 0.0);        
        Lo += (kD * fAlbedo / PI + specular) * radiance * NdotL + vec3(0.01) * fAlbedo * fAO;
    }

    for (int i = 0; i < directionalLightCount; i++)
    {
        vec3 lightDirection = normalize (-directionalLights[i].lightDirection);
        vec3 halfwayDirection = normalize (viewDirection + lightDirection);

        float shadow = CalculateDirectionalLightShadow (i);
        vec3 radiance = CalculateDirectionalLightRadiance (directionalLights[i]);
        vec3 specular = CalculateCookTorranceSpecularBRDF (fNormal, lightDirection, viewDirection, halfwayDirection);
        
        vec3 F0 = vec3(0.04); 
        F0 = mix(F0, fAlbedo, fMetallic);
        vec3 F = fresnelSchlick (max (dot (halfwayDirection, viewDirection), 0.0), F0);

        vec3 kD = vec3 (1.0) - F;
        kD = kD * (1.0 - fMetallic);	

        float NdotL = max (dot (fNormal, lightDirection), 0.0);        
        Lo += ((kD * fAlbedo / PI + specular) * radiance * NdotL + vec3(0.01) * fAlbedo * fAO) * shadow;
    }

    for (int i = 0; i < spotLightCount; i++)
    {
        vec3 lightDirection = normalize (spotLights[i].lightPosition - fPosition);
        vec3 halfwayDirection = normalize (viewDirection + lightDirection);

        vec3 radiance = CalculateSpotLightRadiance (spotLights[i], lightDirection);
        vec3 specular = CalculateCookTorranceSpecularBRDF (fNormal, lightDirection, viewDirection, halfwayDirection);
        
        vec3 F0 = vec3(0.04); 
        F0 = mix(F0, fAlbedo, fMetallic);
        vec3 F = fresnelSchlick (max (dot (halfwayDirection, viewDirection), 0.0), F0);

        vec3 kD = vec3 (1.0) - F;
        kD = kD * (1.0 - fMetallic);	

        float NdotL = max (dot (fNormal, lightDirection), 0.0);        
        Lo += (kD * fAlbedo / PI + specular) * radiance * NdotL + vec3(0.01) * fAlbedo * fAO;
    }

    color = vec4 (Lo, 1.0);

} 

