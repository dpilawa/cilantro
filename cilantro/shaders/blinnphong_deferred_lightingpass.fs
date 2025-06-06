#version %%CILANTRO_GLSL_VERSION%%

/* texture coords */
in vec2 fTextureCoordinates;

/* viewing direction */
vec3 viewDirection;

/* g-buffer */
#if (__VERSION__ >= 420)
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

vec3 fPosition;
vec3 fNormal;
vec3 fDiffuseColor;
vec3 fSpecularColor;
vec3 fEmissiveColor;
float fSpecularShininess;

/* eye position in world space */
uniform vec3 eyePosition;

/* output color */
out vec4 color;

%%include shaders/lights.fs%%
%%include shaders/shadows.fs%%
%%include shaders/blinnphong.fs%%

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

