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
    float lumaM = texture (fScreenTexture, fTextureCoordinates).a;
    float lumaTL = texture (fScreenTexture, fTextureCoordinates + (vec2 (-1.0, 1.0) * fInvResolution)).a;
    float lumaTR = texture (fScreenTexture, fTextureCoordinates + (vec2 (1.0, 1.0) * fInvResolution)).a;
    float lumaBL = texture (fScreenTexture, fTextureCoordinates + (vec2 (-1.0, -1.0) * fInvResolution)).a;
    float lumaBR = texture (fScreenTexture, fTextureCoordinates + (vec2 (1.0, -1.0) * fInvResolution)).a;

    float tmp = (lumaM + lumaTL + lumaTR + lumaBL + lumaBR) * 0.2;
    color = vec4(tmp);
} 