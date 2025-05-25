#version %%CILANTRO_GLSL_VERSION%%

#define MAX_POINT_LIGHTS %%CILANTRO_MAX_POINT_LIGHTS%%
#define MAX_DIRECTIONAL_LIGHTS %%CILANTRO_MAX_DIRECTIONAL_LIGHTS%%
#define MAX_SPOT_LIGHTS %%CILANTRO_MAX_SPOT_LIGHTS%%

const int PCFPOWER = 1;

/* texture coords */
in vec2 fTextureCoordinates;

/* viewing direction */
vec3 viewDirection;

/* material properties */
#if (__VERSION__ >= 430)
layout (binding=0) uniform sampler2D tPosition;
layout (binding=1) uniform sampler2D tNormal;
layout (binding=2) uniform sampler2D tDiffuse;
layout (binding=3) uniform sampler2D tEmissive;
layout (binding=4) uniform sampler2D tSpecular;
#else
uniform sampler2D tPosition;
uniform sampler2D tNormal;
uniform sampler2D tDiffuse;
uniform sampler2D tEmissive;
uniform sampler2D tSpecular;
#endif

/* shadow maps */
#if (__VERSION__ >= 430)
layout (binding = 5) uniform sampler2DArray tDirectionalShadowMap;
#else
uniform sampler2DArray tDirectionalShadowMap;
#endif

vec3 fPosition;
vec3 fNormal;
vec3 fDiffuseColor;
vec3 fSpecularColor;
vec3 fEmissiveColor;
float fSpecularShininess;

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
    highp mat4 mDirectionalLightSpace[MAX_DIRECTIONAL_LIGHTS];
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
highp float CalculateDirectionalLightShadow (int directionalLightIdx)
{
    highp vec4 fragmentLightSpace = mDirectionalLightSpace[directionalLightIdx] * vec4 (fPosition, 1.0);
    highp vec3 depthMapCoords = vec3 (fragmentLightSpace / fragmentLightSpace.w) * 0.5 + 0.5;
    highp float fragmentDepth = abs (depthMapCoords.z);
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

/* calculate contribution of one spot light */
vec4 CalculateSpotLight (SpotLightStruct light, int spotLightIdx)
{
    vec3 lightDirection = normalize (light.lightPosition - fPosition);

    /* check if in cone */
    float theta = dot (lightDirection, normalize (-light.lightDirection));
    float epsilon = light.innerCutoffCosine - light.outerCutoffCosine;
    float intensity = clamp ((theta - light.outerCutoffCosine) / epsilon, 0.0, 1.0); 		

    /* diffuse component */
    float n_dot_l = dot (fNormal, lightDirection);
    float diffuseCoefficient = CalculateDiffuse (n_dot_l);
    
    /* specular component */
    float specularCoefficient = CalculateSpecular (lightDirection, n_dot_l);
    
    /* attenuation */
    float distanceToLight = length (light.lightPosition - fPosition);
    float attenuationFactor = 1.0f / (light.attenuationConst + light.attenuationLinear * distanceToLight + light.attenuationQuadratic * distanceToLight * distanceToLight);
        
    /* aggregate output */
    return vec4 (CalculateOutputColor (diffuseCoefficient, attenuationFactor, specularCoefficient, 1.0, light.lightColor) * intensity, 1.0);
}

void main()
{
    fPosition = texture (tPosition, fTextureCoordinates).xyz;
    fNormal = texture (tNormal, fTextureCoordinates).xyz;
    
    fDiffuseColor = texture (tDiffuse, fTextureCoordinates).rgb;
    fEmissiveColor = texture (tEmissive, fTextureCoordinates).rgb;
    fSpecularColor = texture (tSpecular, fTextureCoordinates).rgb;
    fSpecularShininess = texture (tSpecular, fTextureCoordinates).a;
    
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

