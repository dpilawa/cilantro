#version %%CILANTRO_GLSL_VERSION%%

#if (__VERSION__ < 150)
#extension GL_EXT_geometry_shader4 : enable
#endif

layout(triangles, invocations = $$ACTIVE_POINT_LIGHTS$$) in;
layout(triangle_strip, max_vertices = 18) out;

#if (__VERSION__ >= 420)
layout (std140, binding = %%UBO_POINTLIGHTVIEWMATRICES%%) uniform UniformPointLightViewMatricesBlock
{
    mat4 mLightSpace[%%CILANTRO_MAX_POINT_LIGHTS%% * 6];
};
#else
layout (std140) uniform UniformPointLightViewMatricesBlock
{
    mat4 mLightSpace[%%CILANTRO_MAX_POINT_LIGHTS%% * 6];
};
#endif

uniform int textureArrayOffset;

void main()
{
    for (int f = 0; f < 6; f++)
    {
        for (int i = 0; i < 3; i++)
        {
            gl_Position = mLightSpace[f + 6 * gl_InvocationID] * gl_in[i].gl_Position;
            gl_Layer = textureArrayOffset + f + 6 * gl_InvocationID;
            EmitVertex();
        }
        
        EndPrimitive();
    }
} 