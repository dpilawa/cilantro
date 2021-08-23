#version %%CILANTRO_GL_VERSION%%

/* texture coords */
in vec2 fTextureCoordinates;

/* texture */
uniform sampler2D fScreenTexture;

/* gamma value */
uniform vec2 fInvResolution;

/* output color */
out vec4 color;

void main()
{  
    float FXAA_SPAN_MAX = 8.0;
    float FXAA_REDUCE_MIN = 1.0 / 128.0;
    float FXAA_REDUCE_MUL = 1.0 / 8.0;

    float lumaM = texture (fScreenTexture, fTextureCoordinates).a;
    float lumaTL = texture (fScreenTexture, fTextureCoordinates + (vec2 (-1.0, -1.0) * fInvResolution)).a;
    float lumaTR = texture (fScreenTexture, fTextureCoordinates + (vec2 (1.0, -1.0) * fInvResolution)).a;
    float lumaBL = texture (fScreenTexture, fTextureCoordinates + (vec2 (-1.0, 1.0) * fInvResolution)).a;
    float lumaBR = texture (fScreenTexture, fTextureCoordinates + (vec2 (1.0, 1.0) * fInvResolution)).a;

    vec2 blur;
    blur.x = ((lumaBL + lumaBR) - (lumaTL + lumaTR));
    blur.y = ((lumaTL + lumaBL) - (lumaTR + lumaBR));

    float scale = 1.0 / (min (abs (blur.x), abs (blur.y)) + max ((lumaTL + lumaTR + lumaBL + lumaBR) * 0.25 * FXAA_REDUCE_MUL, FXAA_REDUCE_MIN));
    blur = clamp (blur * scale, vec2 (-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), vec2 (FXAA_SPAN_MAX, FXAA_SPAN_MAX)) * fInvResolution;

    vec4 result1 = (1.0 / 2.0) * (texture (fScreenTexture, fTextureCoordinates + blur * vec2 (1.0 / 3.0 - 0.5)) +
                                   texture (fScreenTexture, fTextureCoordinates + blur * vec2 (2.0 / 3.0 - 0.5)));

    vec4 result2 = result1 * (1.0 / 2.0) + (1.0 / 4.0) * (texture (fScreenTexture, fTextureCoordinates + blur * vec2 (0.0 / 3.0 - 0.5)) +
                                                           texture (fScreenTexture, fTextureCoordinates + blur * vec2 (3.0 / 3.0 - 0.5)));

    float lumaMin = min (lumaM, min (min (lumaTL, lumaTR), min (lumaBL, lumaBR)));
    float lumaMax = max (lumaM, max (max (lumaTL, lumaTR), max (lumaBL, lumaBR)));

    if (result2.a < lumaMin || result2.a > lumaMax)
    {
        color = vec4 (result1.rgb, 1.0);
    }
    else
    {
        color = vec4 (result2.rgb, 1.0);
    }
    
} 