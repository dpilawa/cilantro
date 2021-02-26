#version 140

in vec2 vPosition;
in vec2 vTextureCoordinates;

out vec2 fTextureCoordinates;

void main ()
{
    gl_Position = vec4(vPosition.x, vPosition.y, 0.0, 1.0);
    fTextureCoordinates = vTextureCoordinates;
}
