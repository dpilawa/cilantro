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

vec3 fNormal;
vec3 fAlbedo;
float fMetallic;
float fRoughness;
float fAO;

/* eye position in world space */
uniform vec3 eyePosition;

/* output color */
out vec4 color;

%%include shaders/lights.fs%%
%%include shaders/shadows.fs%%
%%include shaders/pbr.fs%%

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
        Lo += CalculatePointLight (i);
    }

    for (int i = 0; i < directionalLightCount; i++)
    {
        Lo += CalculateDirectionalLight (i);
    }

    for (int i = 0; i < spotLightCount; i++)
    {
        Lo += CalculateSpotLight (i);
    }

    color = vec4 (Lo, 1.0);

} 

