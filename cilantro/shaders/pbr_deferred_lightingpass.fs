#version %%CILANTRO_GLSL_VERSION%%

/* texture coords */
in vec2 fTextureCoordinates;

/* viewing direction */
vec3 viewDirection;

/* g-buffer */
#if (__VERSION__ >= 420)
layout (binding=0) uniform sampler2D tPosition;
layout (binding=1) uniform sampler2D tNormal;
layout (binding=2) uniform sampler2D tAlbedo;
layout (binding=3) uniform sampler2D tMetallicRoughnessAO;
layout (binding=4) uniform sampler2D tUnused;
#else
uniform sampler2D tPosition;
uniform sampler2D tNormal;
uniform sampler2D tAlbedo;
uniform sampler2D tMetallicRoughnessAO;
uniform sampler2D tUnused;
#endif

vec3 fPosition;
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
    
    fPosition = texture (tPosition, fTextureCoordinates).xyz;
    fNormal = texture (tNormal, fTextureCoordinates).xyz;
    fAlbedo = texture (tAlbedo, fTextureCoordinates).rgb;
    fMetallic = texture (tMetallicRoughnessAO, fTextureCoordinates).r;
    fRoughness = texture (tMetallicRoughnessAO, fTextureCoordinates).g;
    fAO = texture (tMetallicRoughnessAO, fTextureCoordinates).b;

    /* calculate viewing direction */
    viewDirection = normalize (eyePosition - fPosition);

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
