#version %%CILANTRO_GL_VERSION%%

/* texture coords */
in vec2 fTextureCoordinates;

/* texture */
uniform sampler2D fScreenTexture;

/* output color */
out vec4 color;

void main()
{
    color = texture (fScreenTexture, fTextureCoordinates);

    /* reinhard tone mapping */
    color.rgb = color.rgb / (color.rgb + vec3 (1.0));

    /* luma */
    color.a = sqrt (dot (color.rgb, vec3 (0.299, 0.587, 0.114)));
} 
