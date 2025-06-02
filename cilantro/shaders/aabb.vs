#version %%CILANTRO_GLSL_VERSION%%

/* vertex data (here already in world space) */
layout (location = 0) in vec3 vPosition;

/* view and projection matrices */
#if (__VERSION__ >= 420)
layout (std140, binding = %%UBO_MATRICES%%) uniform UniformMatricesBlock
{
    mat4 mView;
    mat4 mProjection;
};
#else
layout (std140) uniform UniformMatricesBlock
{
    mat4 mView;
    mat4 mProjection;
};
#endif

void main ()
{
    gl_Position = mProjection * mView * vec4 (vPosition, 1.0);
}

    

