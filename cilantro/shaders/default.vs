#version %%CILANTRO_GL_VERSION%%

/* transformation matrices */
uniform mat4 mModel;
uniform mat3 mNormal;

layout(std140) uniform UniformMatricesBlock
{
    mat4 mView;
    mat4 mProjection;
};

#if (__VERSION__ >= 330)
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUV;
#else
in vec3 vPosition;
in vec3 vNormal;
in vec2 vUV;
#endif

out vec3 fPosition;
out vec3 fNormal;
out vec3 fNormal_View;
out vec2 fUV;

void main ()
{
    gl_Position = mProjection * mView * mModel * vec4 (vPosition.xyz, 1.0);
    
    fPosition = vec3 (mModel * vec4 (vPosition.xyz, 1.0));
    fNormal = normalize (mNormal * vNormal);
    fUV = vUV;
}
    
