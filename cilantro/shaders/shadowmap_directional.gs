#version %%CILANTRO_GLSL_VERSION%%

layout(triangles, invocations = %%ACTIVE_DIRECTIONAL_LIGHTS%%) in;
layout(triangle_strip, max_vertices = 3) out;

layout (std140, binding = %%UBO_DIRECTIONALLIGHTVIEWMATRICES%%) uniform UniformDirectionalLightViewMatricesBlock
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