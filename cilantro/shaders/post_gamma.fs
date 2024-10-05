#version %%CILANTRO_GL_VERSION%%

/* texture coords */
in vec2 fTextureCoordinates;

/* texture */
uniform sampler2D fScreenTexture;

/* gamma value */
uniform float fGamma;

/* output color */
out vec4 color;

void main()
{
    color = texture (fScreenTexture, fTextureCoordinates);
    color.rgb = pow (color.rgb, vec3 (1.0 / fGamma));
} 
    
