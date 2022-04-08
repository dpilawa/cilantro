#version %%CILANTRO_GL_VERSION%%

#define MAX_POINT_LIGHTS %%CILANTRO_MAX_POINT_LIGHTS%%
#define MAX_DIRECTIONAL_LIGHTS %%CILANTRO_MAX_DIRECTIONAL_LIGHTS%%
#define MAX_SPOT_LIGHTS %%CILANTRO_MAX_SPOT_LIGHTS%%

/* fragment position in world space */
in vec3 fPosition;

/* TBN matrix */
in mat3 TBN;

/* texture uv coordinates */
in vec2 fUV;

/* viewing direction */
vec3 viewDirection;

/* material properties */
#if (__VERSION__ >= 420)
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

layout(std140) uniform UniformPointLightsBlock
{
    int pointLightCount;
    PointLightStruct pointLights[MAX_POINT_LIGHTS];
};

layout(std140) uniform UniformDirectionalLightsBlock
{
    int directionalLightCount;
    DirectionalLightStruct directionalLights[MAX_DIRECTIONAL_LIGHTS];
};

layout(std140) uniform UniformSpotLightsBlock
{
    int spotLightCount;
    SpotLightStruct spotLights[MAX_SPOT_LIGHTS];
};

layout (std140) uniform UniformDirectionalLightViewMatricesBlock
{
    mat4 mLightSpace[MAX_DIRECTIONAL_LIGHTS];
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

vec3 CalculateOutputColor (float diffuseCoefficient, float attenuationFactor, float specularCoefficient, vec3 lightColor)
{
    return (diffuseCoefficient * fDiffuseColor * attenuationFactor + specularCoefficient * fSpecularColor) * lightColor;
}

/* calculate contribution of one point light */
vec4 CalculatePointLight (PointLightStruct light)
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
    return vec4 (CalculateOutputColor (diffuseCoefficient, attenuationFactor, specularCoefficient, light.lightColor), 1.0);
}

/* calculate contribution of one directional light */
vec4 CalculateDirectionalLight (DirectionalLightStruct light)
{
    vec3 lightDirection = normalize (-light.lightDirection);

    /* diffuse component */
    float n_dot_l = dot (fNormal, lightDirection);		
    float diffuseCoefficient = CalculateDiffuse (n_dot_l);

    /* specular component */
    float specularCoefficient = CalculateSpecular (lightDirection, n_dot_l);

    /* attenuation */
    float attenuationFactor = 1.0f;

    /* aggregate output */
    return vec4 (CalculateOutputColor (diffuseCoefficient, attenuationFactor, specularCoefficient, light.lightColor), 1.0);
}

/* calculate contribution of one spot light */
vec4 CalculateSpotLight (SpotLightStruct light)
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
    return vec4 (CalculateOutputColor (diffuseCoefficient, attenuationFactor, specularCoefficient, light.lightColor) * intensity, 1.0);
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
        color += CalculatePointLight (pointLights[i]);
    }

    for (int i=0; i < directionalLightCount; i++)
    {
        color += CalculateDirectionalLight (directionalLights[i]);
    }

    for (int i=0; i < spotLightCount; i++)
    {
        color += CalculateSpotLight (spotLights[i]);
    }		
} 