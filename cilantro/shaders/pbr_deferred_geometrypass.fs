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

/* output g-buffer */
layout (location=0) out vec4 gPosition;
layout (location=1) out vec4 gNormal;
layout (location=2) out vec4 gAlbedo;
layout (location=3) out vec4 gMetallicRoughnessAO;

void main()
{
    /* fragment position */
    gPosition.xyz = fPosition;

    /* fragment normal */
    gNormal.xyz = normalize (TBN * (texture (tNormal, fUV).rgb * 2.0 - 1.0));

    /* pbr material properties */
    gAlbedo.rgb = texture (tAlbedo, fUV).rgb;
    gMetallicRoughnessAO.r = texture (tMetallic, fUV).r;
    gMetallicRoughnessAO.g = texture (tRoughness, fUV).r;
    gMetallicRoughnessAO.b = texture (tAO, fUV).r;

} 

