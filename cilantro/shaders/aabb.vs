#version %%CILANTRO_GLSL_VERSION%%

/* vertex data (here already in world space) */
layout (location = 0) in vec3 vPosition;

/* view and projection matrices */
layout (std140, binding = %%UBO_MATRICES%%) uniform UniformMatricesBlock
{
    mat4 mView;
    mat4 mProjection;
};

void main ()
{
    gl_Position = mProjection * mView * vec4 (vPosition, 1.0);
}

    

