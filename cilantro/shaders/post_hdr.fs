#version 140

/* texture coords */
in vec2 fTextureCoordinates;

/* output color */
out vec4 color;

/* texture */
uniform sampler2D fScreenTexture;

void main()
{
    color = texture (fScreenTexture, fTextureCoordinates);
    color = color / (color + vec4 (1.0));
} 
