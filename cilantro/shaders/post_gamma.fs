#version 130

/* texture coords */
in vec2 fTextureCoordinates;

/* gamma value */
uniform float fGamma;

/* output color */
out vec4 color;

/* texture */
uniform sampler2D fScreenTexture;

void main()
{
    color = texture (fScreenTexture, fTextureCoordinates);
    color.rgb = pow (color.rgb, vec3 (1.0 / fGamma));
} 
    
