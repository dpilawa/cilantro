#version %%CILANTRO_GL_VERSION%%

/* material properties */
#if (__VERSION__ >= 420)
layout (binding=0) uniform sampler2D tPosition;
layout (binding=1) uniform sampler2D tNormal;
layout (binding=2) uniform sampler2D tMetallic;
layout (binding=3) uniform sampler2D tRoughness;
layout (binding=4) uniform sampler2D tAlbedoAO;
#else
uniform sampler2D tPosition;
uniform sampler2D tNormal;
uniform sampler2D tMetallic;
uniform sampler2D tRoughness;
uniform sampler2D tAlbedoAO;
#endif

/* output color */
out vec4 color;

void main()
{
    color = vec4 (1.0, 0.0, 0.0, 1.0);
} 

