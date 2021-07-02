#version %%CILANTRO_GL_VERSION%%

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

/* eye position in world space */
uniform vec3 eyePosition;

/* output g-buffer */
layout (location=0) out vec3 gPosition;
layout (location=1) out vec3 gNormal;
layout (location=2) out vec3 gMetallic;
layout (location=3) out vec3 gRoughness;
layout (location=4) out vec4 gAlbedoAO;

void main()
{
    /* fragment position */
    gPosition = fPosition;

    /* fragment normal */
    gNormal = normalize (TBN * (texture (tNormal, fUV).rgb * 2.0 - 1.0));

    /* prb material properties */
    gMetallic.rgb = texture (tMetallic, fUV).rgb;
    gRoughness.rgb = texture (tRoughness, fUV).rgb;
    gAlbedoAO.rgb = texture (tAlbedo, fUV).rgb;
    gAlbedoAO.a = texture (tAO, fUV).a;

} 

