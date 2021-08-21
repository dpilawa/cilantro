#version %%CILANTRO_GL_VERSION%%

/* material properties */
#if (__VERSION__ >= 420)
layout (binding=0) uniform sampler2D tPosition;
layout (binding=1) uniform sampler2D tNormal;
layout (binding=2) uniform sampler2D tEmissive;
layout (binding=3) uniform sampler2D tUnused;
layout (binding=4) uniform sampler2D tSpecular;
#else
uniform sampler2D tPosition;
uniform sampler2D tNormal;
uniform sampler2D tEmissive;
uniform sampler2D tUnused;
uniform sampler2D tSpecular;
#endif

/* output color */
out vec4 color;

void main()
{
    color = vec4 (0.0, 1.0, 0.0, 1.0);
} 

