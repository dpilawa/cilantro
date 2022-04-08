#version %%CILANTRO_GL_VERSION%%

layout(triangles, invocations = %%ACTIVE_DIRECTIONAL_LIGHTS%%) in;
layout(triangle_strip, max_vertices = 3) out;

layout (std140) uniform UniformDirectionalLightViewMatricesBlock
{
    mat4 mLightSpace[%%CILANTRO_MAX_DIRECTIONAL_LIGHTS%%];
};
    
void main()
{          
    for (int i = 0; i < 3; ++i)
    {
        gl_Position = mLightSpace[gl_InvocationID] * gl_in[i].gl_Position;
        gl_Layer = gl_InvocationID;
        EmitVertex();
    }
    
    EndPrimitive();
} 