#version %%CILANTRO_GL_VERSION%%
#extension GL_EXT_geometry_shader : enable
#extension GL_ARB_tessellation_shader : enable
#extension GL_ARB_gpu_shader5 : enable
#extension GL_ARB_geometry_shader4 : enable

layout(triangles, invocations = %%ACTIVE_DIRECTIONAL_LIGHTS%%) in;
layout(triangle_strip, max_vertices = 3) out;

layout (std140) uniform UniformDirectionalLightViewMatricesBlock
{
    mat4 mLightSpace[%%CILANTRO_MAX_DIRECTIONAL_LIGHTS%%];
};

void main()
{
    for (int i = 2; i >= 0; --i)
    {
        gl_Position = mLightSpace[gl_InvocationID] * gl_in[i].gl_Position;
        gl_Layer = gl_InvocationID;
        EmitVertex();
    }
    
    EndPrimitive();
} 