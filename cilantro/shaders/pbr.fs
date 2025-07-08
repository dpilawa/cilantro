const float PI = 3.14159265359;

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow (1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max (dot (N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max (dot (N, V), 0.0);
    float NdotL = max (dot (N, L), 0.0);
    float ggx2 = GeometrySchlickGGX (NdotV, roughness);
    float ggx1 = GeometrySchlickGGX (NdotL, roughness);
    
    return ggx1 * ggx2;
}

vec3 CalculateCookTorranceSpecularBRDF (vec3 N, vec3 L, vec3 V, vec3 H)
{
    vec3 F0 = vec3 (0.04); 
    F0 = mix (F0, fAlbedo, fMetallic);
    vec3 F = fresnelSchlick (max (dot (H, V), 0.0), F0);

    float NDF = DistributionGGX (N, H, fRoughness);       
    float G = GeometrySmith (N, V, L, fRoughness);       

    return (NDF * G * F) / max (4.0 * max (dot (N, V), 0.0) * max (dot (N, L), 0.0), 0.001);
}

/* calculate point light radiance */
vec3 CalculatePointLightRadiance (PointLightStruct light)
{
    /* attenuation */
    float distanceToLight = length (light.lightPosition - fPosition);
    float attenuationFactor = 1.0f / (light.attenuationConst + light.attenuationLinear * distanceToLight + light.attenuationQuadratic * distanceToLight * distanceToLight);

    /* radiance */
    return attenuationFactor * light.lightColor;
}

/* calculate directional light radiance */
vec3 CalculateDirectionalLightRadiance (DirectionalLightStruct light)
{
    /* radiance (directional light has no attenuation) */
    return light.lightColor;
}

/* calculate spot light radiance */
vec3 CalculateSpotLightRadiance (SpotLightStruct light, vec3 L)
{
    /* check if in cone */
    float theta = dot (L, normalize (-light.lightDirection));
    float epsilon = light.innerCutoffCosine - light.outerCutoffCosine;
    float intensity = clamp ((theta - light.outerCutoffCosine) / epsilon, 0.0, 1.0); 		
    
    /* attenuation */
    float distanceToLight = length (light.lightPosition - fPosition);
    float attenuationFactor = 1.0f / (light.attenuationConst + light.attenuationLinear * distanceToLight + light.attenuationQuadratic * distanceToLight * distanceToLight);
        
    /* radiance */
    return intensity * attenuationFactor * light.lightColor;
}

vec3 CalculatePointLight (int pointLightIdx)
{
    vec3 lightDirection = normalize (pointLights[pointLightIdx].lightPosition - fPosition);
    vec3 halfwayDirection = normalize (viewDirection + lightDirection);

    float shadow = CalculatePointLightShadow (pointLightIdx);
    vec3 radiance = CalculatePointLightRadiance (pointLights[pointLightIdx]);
    vec3 specular = CalculateCookTorranceSpecularBRDF (fNormal, lightDirection, viewDirection, halfwayDirection);
    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, fAlbedo, fMetallic);
    vec3 F = fresnelSchlick (max (dot (halfwayDirection, viewDirection), 0.0), F0);

    vec3 kD = vec3 (1.0) - F;
    kD = kD * (1.0 - fMetallic);	

    float NdotL = max (dot (fNormal, lightDirection), 0.0);        
    return (kD * fAlbedo / PI + specular) * radiance * shadow * NdotL + vec3(0.01) * fAlbedo * fAO;
}

vec3 CalculateDirectionalLight (int directionalLightIdx)
{
    vec3 lightDirection = normalize (-directionalLights[directionalLightIdx].lightDirection);
    vec3 halfwayDirection = normalize (viewDirection + lightDirection);

    float shadow = CalculateDirectionalLightShadow (directionalLightIdx);
    vec3 radiance = CalculateDirectionalLightRadiance (directionalLights[directionalLightIdx]);
    vec3 specular = CalculateCookTorranceSpecularBRDF (fNormal, lightDirection, viewDirection, halfwayDirection);
    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, fAlbedo, fMetallic);
    vec3 F = fresnelSchlick (max (dot (halfwayDirection, viewDirection), 0.0), F0);

    vec3 kD = vec3 (1.0) - F;
    kD = kD * (1.0 - fMetallic);	

    float NdotL = max (dot (fNormal, lightDirection), 0.0);        
    return ((kD * fAlbedo / PI + specular) * radiance * shadow * NdotL + vec3(0.01) * fAlbedo * fAO) * shadow;
}

vec3 CalculateSpotLight (int spotLightIdx)
{
    vec3 lightDirection = normalize (spotLights[spotLightIdx].lightPosition - fPosition);
    vec3 halfwayDirection = normalize (viewDirection + lightDirection);

    float shadow = CalculateSpotLightShadow (spotLightIdx);
    vec3 radiance = CalculateSpotLightRadiance (spotLights[spotLightIdx], lightDirection);
    vec3 specular = CalculateCookTorranceSpecularBRDF (fNormal, lightDirection, viewDirection, halfwayDirection);
    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, fAlbedo, fMetallic);
    vec3 F = fresnelSchlick (max (dot (halfwayDirection, viewDirection), 0.0), F0);

    vec3 kD = vec3 (1.0) - F;
    kD = kD * (1.0 - fMetallic);	

    float NdotL = max (dot (fNormal, lightDirection), 0.0);        
    return (kD * fAlbedo / PI + specular) * radiance * shadow * NdotL + vec3(0.01) * fAlbedo * fAO;
}