/* calculate diffuse coefficient */
float CalculateDiffuse (float n_dot_l)
{
    return clamp (n_dot_l, 0.0, 1.0);
}

/* calculate specular coefficient */
float CalculateSpecular (vec3 lightDirection, float n_dot_l)
{
    vec3 halfwayDirection = normalize (lightDirection + viewDirection);
    return pow (clamp (dot (fNormal, halfwayDirection), 0.0, 1.0), fSpecularShininess) * smoothstep(0.0, 0.5, n_dot_l);		
}

vec3 CalculateOutputColor (float diffuseCoefficient, float attenuationFactor, float specularCoefficient, float shadow, vec3 lightColor)
{
    return (diffuseCoefficient * fDiffuseColor * shadow * attenuationFactor + specularCoefficient * fSpecularColor) * lightColor;
}

/* calculate contribution of one point light */
vec4 CalculatePointLight (PointLightStruct light, int pointLightIdx)
{
    vec3 lightDirection = normalize (light.lightPosition - fPosition);

    /* diffuse component */
    float n_dot_l = dot (fNormal, lightDirection);
    float diffuseCoefficient = CalculateDiffuse (n_dot_l);
    
    /* specular component */
    float specularCoefficient = CalculateSpecular (lightDirection, n_dot_l);
    
    /* attenuation */
    float distanceToLight = length (light.lightPosition - fPosition);
    float attenuationFactor = 1.0f / (light.attenuationConst + light.attenuationLinear * distanceToLight + light.attenuationQuadratic * distanceToLight * distanceToLight);

    /* aggregate output */
    return vec4 (CalculateOutputColor (diffuseCoefficient, attenuationFactor, specularCoefficient, 1.0, light.lightColor), 1.0);
}

/* calculate contribution of one directional light */
vec4 CalculateDirectionalLight (DirectionalLightStruct light, int directionalLightIdx)
{
    vec3 lightDirection = normalize (-light.lightDirection);

    /* diffuse component */
    float n_dot_l = dot (fNormal, lightDirection);		
    float diffuseCoefficient = CalculateDiffuse (n_dot_l);

    /* shadow */
    float shadow = CalculateDirectionalLightShadow (directionalLightIdx);

    /* specular component */
    float specularCoefficient = CalculateSpecular (lightDirection, n_dot_l);

    /* attenuation */
    float attenuationFactor = 1.0f;

    /* aggregate output */
    return vec4 (CalculateOutputColor (diffuseCoefficient, attenuationFactor, specularCoefficient, shadow, light.lightColor), 1.0);
}

/* calculate contribution of one spot light */
vec4 CalculateSpotLight (SpotLightStruct light, int spotLightIdx)
{
    vec3 lightDirection = normalize (light.lightPosition - fPosition);

    /* check if in cone */
    float theta = dot (lightDirection, normalize (-light.lightDirection));
    float epsilon = light.innerCutoffCosine - light.outerCutoffCosine;
    float intensity = clamp ((theta - light.outerCutoffCosine) / epsilon, 0.0, 1.0); 		

    /* shadow */
    float shadow = CalculateSpotLightShadow (spotLightIdx);

    /* diffuse component */
    float n_dot_l = dot (fNormal, lightDirection);
    float diffuseCoefficient = CalculateDiffuse (n_dot_l);
    
    /* specular component */
    float specularCoefficient = CalculateSpecular (lightDirection, n_dot_l);
    
    /* attenuation */
    float distanceToLight = length (light.lightPosition - fPosition);
    float attenuationFactor = 1.0f / (light.attenuationConst + light.attenuationLinear * distanceToLight + light.attenuationQuadratic * distanceToLight * distanceToLight);
        
    /* aggregate output */
    return vec4 (CalculateOutputColor (diffuseCoefficient, attenuationFactor, specularCoefficient, shadow, light.lightColor) * intensity, 1.0);
}
