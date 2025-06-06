#version %%CILANTRO_GLSL_VERSION%%

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

/* output color */
out vec4 color;

%%include shaders/lights.fs%%
%%include shaders/shadows.fs%%
%%include shaders/blinnphong.fs%%

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