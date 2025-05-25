#version %%CILANTRO_GLSL_VERSION%%

#if (__VERSION__ >= 330)
layout (location = 0) in vec2 vPosition;
layout (location = 2) in vec2 vTextureCoordinates;
#else
in vec2 vPosition;
in vec2 vTextureCoordinates;
#endif

out vec2 fTextureCoordinates;

void main ()
{
    gl_Position = vec4 (vPosition.x, vPosition.y, 0.0, 1.0);
    fTextureCoordinates = vTextureCoordinates;
}

