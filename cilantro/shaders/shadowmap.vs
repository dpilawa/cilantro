#version %%CILANTRO_GL_VERSION%%

/* vertex data */
#if (__VERSION__ >= 330)
layout (location = 0) in vec3 vPosition;
#else
in vec3 vPosition;
#endif
    
/* transformation matrices */
uniform mat4 mModel;
    
void main()
{
    gl_Position = mModel * vec4 (vPosition, 1.0);
}