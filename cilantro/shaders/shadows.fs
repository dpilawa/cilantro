/* shadow maps */

#if (__VERSION__ >= 420)
layout (binding = 5) uniform sampler2DArrayShadow tShadowMap;
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

/* calculate directional light shadow */
float CalculateDirectionalLightShadow (int directionalLightIdx)
{
    vec4 fragmentLightSpace = mDirectionalLightSpace[directionalLightIdx] * vec4(fPosition, 1.0);
    vec3 depthMapCoords = fragmentLightSpace.xyz / fragmentLightSpace.w * 0.5 + 0.5;

    if (any(lessThan(depthMapCoords, vec3(0.0))) || any(greaterThan(depthMapCoords, vec3(1.0))))
        return 1.0;

    float shadow = texture(tShadowMap, vec4(depthMapCoords.xy, directionalLightIdx, depthMapCoords.z - %%CILANTRO_SHADOW_BIAS%%));
    return shadow;
}

/* calculate spot light shadow */
float CalculateSpotLightShadow (int spotLightIdx)
{
    vec4 fragmentLightSpace = mSpotLightSpace[spotLightIdx] * vec4(fPosition, 1.0);
    vec3 depthMapCoords = fragmentLightSpace.xyz / fragmentLightSpace.w * 0.5 + 0.5;

    if (any(lessThan(depthMapCoords, vec3(0.0))) || any(greaterThan(depthMapCoords, vec3(1.0))))
        return 1.0;

    float shadow = texture(tShadowMap, vec4(depthMapCoords.xy, directionalLightCount + spotLightIdx, depthMapCoords.z - %%CILANTRO_SHADOW_BIAS%%));
    return shadow;
}