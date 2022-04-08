#version %%CILANTRO_GL_VERSION%%

/* vertex data */
#if (__VERSION__ >= 330)
layout (location = 0) in vec3 vPosition;
layout (location = 5) in ivec4 vBoneIndices;
layout (location = 6) in vec4 vBoneWeights;
#else
in vec3 vPosition;
in ivec4 vBoneIndices;
in vec4 vBoneWeights;
#endif
    
/* transformation matrices */
uniform mat4 mModel;

/* array of bone transformation matrices */
uniform mat4 mBoneTransformations[%%CILANTRO_MAX_BONES%%];

void main()
{
    vec4 transformedPosition = vec4 (0.0);
    
    for (int i = 0; i < %%CILANTRO_MAX_BONE_INFLUENCES%%; i++)
    {
        mat4 boneTransform = mBoneTransformations[vBoneIndices[i]];
        transformedPosition += boneTransform * vec4 (vPosition, 1.0) * vBoneWeights[i];
    }

    gl_Position = mModel * transformedPosition;
}