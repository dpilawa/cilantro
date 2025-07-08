/* flags */
uniform int shadowMapEnabled;

/* shadow maps */

#if (__VERSION__ < 400)
#extension ARB_texture_cube_map_array : enable
#endif 

#if (__VERSION__ >= 420)
layout (binding = %%CILANTRO_SHADOW_MAP_BINDING%%) uniform sampler2DArrayShadow tShadowMap;
#else
uniform sampler2DArrayShadow tShadowMap;
#endif

/* light view matrices */

#if (__VERSION__ >= 420)
layout (std140, binding = %%UBO_DIRECTIONALLIGHTVIEWMATRICES%%) uniform UniformDirectionalLightViewMatricesBlock
{
    mat4 mDirectionalLightSpace[%%CILANTRO_MAX_DIRECTIONAL_LIGHTS%%];
};
#else
layout (std140) uniform UniformDirectionalLightViewMatricesBlock
{
    mat4 mDirectionalLightSpace[%%CILANTRO_MAX_DIRECTIONAL_LIGHTS%%];
};
#endif

#if (__VERSION__ >= 420)
layout (std140, binding = %%UBO_SPOTLIGHTVIEWMATRICES%%) uniform UniformSpotLightViewMatricesBlock
{
    mat4 mSpotLightSpace[%%CILANTRO_MAX_SPOT_LIGHTS%%];
};
#else
layout (std140) uniform UniformSpotLightViewMatricesBlock
{
    mat4 mSpotLightSpace[%%CILANTRO_MAX_SPOT_LIGHTS%%];
};
#endif

#if (__VERSION__ >= 420)
layout (std140, binding = %%UBO_POINTLIGHTVIEWMATRICES%%) uniform UniformPointLightViewMatricesBlock
{
    mat4 mPointLightSpace[6 * %%CILANTRO_MAX_POINT_LIGHTS%%];
};
#else
layout (std140) uniform UniformPointLightViewMatricesBlock
{
    mat4 mPointLightSpace[6 * %%CILANTRO_MAX_POINT_LIGHTS%%];
};
#endif

/* calculate directional light shadow */
float CalculateDirectionalLightShadow (int directionalLightIdx)
{
    if (shadowMapEnabled == 0)
        return 1.0;

    vec4 fragmentLightSpace = mDirectionalLightSpace[directionalLightIdx] * vec4 (fPosition, 1.0);
    vec3 depthMapCoords = fragmentLightSpace.xyz / fragmentLightSpace.w * 0.5 + 0.5;

    if (any (lessThan (depthMapCoords, vec3 (0.0))) || any (greaterThan (depthMapCoords, vec3 (1.0))))
        return 1.0;

    return texture (tShadowMap, vec4 (depthMapCoords.xy, directionalLightIdx, depthMapCoords.z - %%CILANTRO_SHADOW_BIAS%%));
}

/* calculate spot light shadow */
float CalculateSpotLightShadow (int spotLightIdx)
{
    if (shadowMapEnabled == 0)
        return 1.0;

    vec4 fragmentLightSpace = mSpotLightSpace[spotLightIdx] * vec4 (fPosition, 1.0);
    vec3 depthMapCoords = fragmentLightSpace.xyz / fragmentLightSpace.w * 0.5 + 0.5;

    if (any (lessThan (depthMapCoords, vec3 (0.0))) || any (greaterThan (depthMapCoords, vec3 (1.0))))
        return 1.0;

    return texture (tShadowMap, vec4 (depthMapCoords.xy, directionalLightCount + spotLightIdx, depthMapCoords.z - %%CILANTRO_SHADOW_BIAS%%));
}

/* calculate point light shadow */
float CalculatePointLightShadow (int pointLightIdx)
{
    if (shadowMapEnabled == 0)
        return 1.0;

    vec3 lightPos = pointLights[pointLightIdx].lightPosition;
    vec3 lightDir = normalize (fPosition - lightPos);

    // Determine which cube face corresponds to the direction
    int faceIndex = 0;
    vec3 absDir = abs (lightDir);
    if (absDir.x >= absDir.y && absDir.x >= absDir.z) {
        faceIndex = lightDir.x > 0.0 ? 0 : 1;
    } else if (absDir.y >= absDir.x && absDir.y >= absDir.z) {
        faceIndex = lightDir.y > 0.0 ? 2 : 3;
    } else {
        faceIndex = lightDir.z > 0.0 ? 4 : 5;
    }

    // Transform fragment pos to light clip space using corresponding face matrix
    mat4 lightSpaceMatrix = mPointLightSpace[faceIndex + 6 * pointLightIdx];
    vec4 fragmentLightSpace = lightSpaceMatrix * vec4 (fPosition, 1.0);

    // Compute normalized depth: map from clip space z/w [-1..1] to [0..1]
    vec3 depthMapCoords = fragmentLightSpace.xyz / fragmentLightSpace.w * 0.5 + 0.5;

    if (any (lessThan (depthMapCoords, vec3 (0.0))) || any (greaterThan (depthMapCoords, vec3 (1.0))))
        return 1.0;

    return texture (tShadowMap, vec4 (depthMapCoords.xy, directionalLightCount + spotLightCount + 6 * pointLightIdx + faceIndex, depthMapCoords.z - %%CILANTRO_SHADOW_BIAS%%));
}
