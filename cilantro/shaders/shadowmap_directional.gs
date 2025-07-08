#version %%CILANTRO_GLSL_VERSION%%

#if (__VERSION__ < 150)
#extension GL_EXT_geometry_shader4 : enable
#endif

layout(triangles, invocations = $$ACTIVE_DIRECTIONAL_LIGHTS$$) in;
layout(triangle_strip, max_vertices = 3) out;

#if (__VERSION__ >= 420)
layout (std140, binding = %%UBO_DIRECTIONALLIGHTVIEWMATRICES%%) uniform UniformDirectionalLightViewMatricesBlock
{
    mat4 mLightSpace[%%CILANTRO_MAX_DIRECTIONAL_LIGHTS%%];
};
#else
layout (std140) uniform UniformDirectionalLightViewMatricesBlock
{
    mat4 mLightSpace[%%CILANTRO_MAX_DIRECTIONAL_LIGHTS%%];
};
#endif

uniform int textureArrayOffset;

void main()
{
    for (int i = 0; i < 3; i++)
    {
        gl_Position = mLightSpace[gl_InvocationID] * gl_in[i].gl_Position;
        gl_Layer = textureArrayOffset + gl_InvocationID;
        EmitVertex();
    }
    
    EndPrimitive();
} 