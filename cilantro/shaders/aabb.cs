#version %%CILANTRO_GL_VERSION%%

layout(local_size_x = %%CILANTRO_COMPUTE_GROUP_SIZE%%) in;

/* transformation matrices */
uniform mat4 mModel;

layout(std430) readonly buffer VertexBufferBlock {
    float positions[];
};

layout(std430) buffer BoneIndicesBufferBlock {
    uvec4 boneIndices[];
};

layout(std430) buffer BoneWeightsBufferBlock {
    vec4 boneWeights[];
};

layout(std140) uniform UniformBoneTransformationsBlock {
    mat4 mBoneTransformations[%%CILANTRO_MAX_BONES%%];
};

layout(std430) buffer AABBBufferBlock {
    uvec3 minBits;
    uint pad1;
    uvec3 maxBits;
    uint pad2;
};

shared vec3 sharedMin[%%CILANTRO_COMPUTE_GROUP_SIZE%%];
shared vec3 sharedMax[%%CILANTRO_COMPUTE_GROUP_SIZE%%];

// order-preserving float-to-uint encoding
uint floatToOrderedUint(float val) {
    uint bits = floatBitsToUint(val);
    return (bits & 0x80000000u) != 0u
        ? ~bits
        : bits | 0x80000000u;
}

vec4 getPosition(uint idx) {
    uint i = idx * 3;
    return vec4 (positions[i], positions[i + 1], positions[i + 2], 1.0);
}

void main() {
    uint gid = gl_GlobalInvocationID.x;
    uint lid = gl_LocalInvocationID.x;

    vec4 pos = getPosition(gid);
    uvec4 indices = boneIndices[gid];
    vec4 weights = boneWeights[gid];

    vec4 skinned = vec4(0.0);

    for (int i = 0; i < %%CILANTRO_MAX_BONE_INFLUENCES%%; ++i) {
        skinned += mBoneTransformations[indices[i]] * pos * weights[i];
    }

    vec4 skinnedWorld = mModel * skinned;

    sharedMin[lid] = skinnedWorld.xyz / skinnedWorld.w;
    sharedMax[lid] = skinnedWorld.xyz / skinnedWorld.w;
    barrier();

    for (uint offset = %%CILANTRO_COMPUTE_GROUP_SIZE%% >> 1; offset > 0; offset >>= 1) {
        if (lid < offset) {
            sharedMin[lid] = min(sharedMin[lid], sharedMin[lid + offset]);
            sharedMax[lid] = max(sharedMax[lid], sharedMax[lid + offset]);
        }
        barrier();
    }

    if (lid == 0) {
        atomicMin(minBits.x, floatToOrderedUint(sharedMin[0].x));
        atomicMin(minBits.y, floatToOrderedUint(sharedMin[0].y));
        atomicMin(minBits.z, floatToOrderedUint(sharedMin[0].z));

        atomicMax(maxBits.x, floatToOrderedUint(sharedMax[0].x));
        atomicMax(maxBits.y, floatToOrderedUint(sharedMax[0].y));
        atomicMax(maxBits.z, floatToOrderedUint(sharedMax[0].z));
    }
}
