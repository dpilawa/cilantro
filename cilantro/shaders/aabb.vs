#version %%CILANTRO_GL_VERSION%%

/* vertex data (here already in world space) */
#if (__VERSION__ >= 330)
layout (location = 0) in vec3 vPosition;
#else
in vec3 vPosition;
#endif

/* view and projection matrices */
layout (std140) uniform UniformMatricesBlock
{
    mat4 mView;
    mat4 mProjection;
};

void main ()
{
    gl_Position = mProjection * mView * vec4 (vPosition, 1.0);
}

    

