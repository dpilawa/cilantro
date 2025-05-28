#version %%CILANTRO_GLSL_VERSION%%

layout(triangles, invocations = %%ACTIVE_SPOT_LIGHTS%%) in;
layout(triangle_strip, max_vertices = 3) out;

layout (std140, binding = %%UBO_SPOTLIGHTVIEWMATRICES%%) uniform UniformSpotLightViewMatricesBlock
{
    mat4 mLightSpace[%%CILANTRO_MAX_SPOT_LIGHTS%%];
};

uniform int textureArrayOffset;

void main()
{
    for (int i = 2; i >= 0; --i)
    {
        gl_Position = mLightSpace[gl_InvocationID] * gl_in[i].gl_Position;
        gl_Layer = textureArrayOffset + gl_InvocationID;
        EmitVertex();
    }
    
    EndPrimitive();
} 