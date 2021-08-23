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

uniform float fSpecularShininess;

/* eye position in world space */
uniform vec3 eyePosition;

/* output g-buffer */
layout (location=0) out vec4 gPosition;
layout (location=1) out vec4 gNormal;
layout (location=2) out vec4 gDiffuse;
layout (location=3) out vec4 gEmissive;
layout (location=4) out vec4 gSpecular;

void main()
{
    /* fragment position */
    gPosition.xyz = fPosition;

    /* fragment normal */
    gNormal.xyz = normalize (TBN * (texture (tNormal, fUV).rgb * 2.0 - 1.0));

    /* pbr material properties */
    gDiffuse.rgb = texture (tDiffuse, fUV).rgb;
    gEmissive.rgb = texture (tEmissive, fUV).rgb;
    gSpecular.rgb = texture (tSpecular, fUV).rgb;
    gSpecular.a = fSpecularShininess;

} 

