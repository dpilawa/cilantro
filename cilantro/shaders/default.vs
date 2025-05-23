#version %%CILANTRO_GL_VERSION%%

/* vertex data */
#if (__VERSION__ >= 330)
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUV;
layout (location = 3) in vec3 vTangent;
layout (location = 4) in vec3 vBitangent;
layout (location = 5) in uvec4 vBoneIndices;
layout (location = 6) in vec4 vBoneWeights;
#else
in vec3 vPosition;
in vec3 vNormal;
in vec2 vUV;
in vec3 vTangent;
in vec3 vBitangent;
in uvec4 vBoneIndices;
in vec4 vBoneWeights;
#endif

/* transformation matrices */
uniform mat4 mModel;
uniform mat3 mNormal;

/* view and projection matrices */
layout (std140) uniform UniformMatricesBlock
{
    mat4 mView;
    mat4 mProjection;
};

/* array of bone transformation matrices */
layout (std140) uniform UniformBoneTransformationsBlock {
    mat4 mBoneTransformations[%%CILANTRO_MAX_BONES%%];
};

/* output variables */
out vec3 fPosition;
out vec2 fUV;
out mat3 TBN;

void main ()
{
    vec4 transformedPosition = vec4 (0.0);
    vec4 transformedNormal = vec4 (0.0);
    vec4 transformedTangent = vec4 (0.0);
    vec4 transformedBitangent = vec4 (0.0);

    for (int i = 0; i < %%CILANTRO_MAX_BONE_INFLUENCES%%; i++)
    {
        mat4 boneTransform = mBoneTransformations[vBoneIndices[i]];
        transformedPosition += boneTransform * vec4 (vPosition, 1.0) * vBoneWeights[i];
        transformedNormal += boneTransform * vec4 (vNormal, 0.0) * vBoneWeights[i];
        transformedTangent += boneTransform * vec4 (vTangent, 0.0) * vBoneWeights[i];
        transformedBitangent += boneTransform * vec4 (vBitangent, 0.0) * vBoneWeights[i];
    }

    gl_Position = mProjection * mView * mModel * transformedPosition;
    
    /* calculate TBN matrix */
    vec3 T = normalize (mNormal * vec3 (transformedTangent));
    vec3 N = normalize (mNormal * vec3 (transformedNormal));
    T = normalize (T - dot (T, N) * N);
    vec3 B = cross (N, T);

    if (dot (cross (N, T), B) < 0.0)
    {
        T = -T;
    }

    TBN = mat3 (T, B, N);

    /* world space vertex position */
    fPosition = vec3 (mModel * transformedPosition);

    /* texture coordinates */
    fUV = vUV;
}
    

