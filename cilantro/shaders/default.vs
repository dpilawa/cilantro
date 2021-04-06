#version %%CILANTRO_GL_VERSION%%

/* vertex data */
#if (__VERSION__ >= 330)
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUV;
layout (location = 3) in vec3 vTangent;
layout (location = 4) in vec3 vBitangent;
#else
in vec3 vPosition;
in vec3 vNormal;
in vec2 vUV;
in vec3 vTangent;
in vec3 vBitangent;
#endif

/* transformation matrices */
uniform mat4 mModel;
uniform mat3 mNormal;

layout(std140) uniform UniformMatricesBlock
{
    mat4 mView;
    mat4 mProjection;
};

/* output variables */
out vec3 fPosition;
out vec2 fUV;
out mat3 TBN;

void main ()
{
    gl_Position = mProjection * mView * mModel * vec4 (vPosition.xyz, 1.0);
    
    /* calculate TBN matrix */
    vec3 T = normalize (mNormal * vTangent);
    vec3 N = normalize (mNormal * vNormal);
    T = normalize (T - dot (T, N) * N);
    vec3 B = cross (N, T);
    TBN = mat3 (T, B, N);

    /* world space vertex position */
    fPosition = vec3 (mModel * vec4 (vPosition.xyz, 1.0));

    fUV = vUV;
}
    

