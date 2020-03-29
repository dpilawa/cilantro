#ifndef _POST_HDR_FS_H_
#define _POST_HDR_FS_H_

#include <string>

#define xstr(a) str(a)
#define str(a) #a

std::string gPostHDRFragmentShader = 
R"(#version )"
xstr(CILANTRO_GL_VERSION)
R"V0G0N(

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
    
)V0G0N";

#endif