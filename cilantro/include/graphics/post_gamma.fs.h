#ifndef _POST_GAMMA_FS_H_
#define _POST_GAMMA_FS_H_

#include <string>

#define xstr(a) str(a)
#define str(a) #a

std::string gPostGammaFragmentShader = 
R"(#version )"
xstr(CILANTRO_MIN_GL_VERSION)
R"V0G0N(

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
    
)V0G0N";

#endif