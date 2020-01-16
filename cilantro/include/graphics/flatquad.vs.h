#ifndef _FLATQUAD_VS_H_
#define _FLATQUAD_VS_H_

#include <string>

#define xstr(a) str(a)
#define str(a) #a

std::string gFlatQuadVertexShader = 
R"(#version )"
xstr(CILANTRO_MIN_GL_VERSION)
#if (CILANTRO_MIN_GL_VERSION >= 330)
R"V0G0N(
    layout (location = 0) in vec2 vPosition;
    layout (location = 1) in vec2 vTextureCoordinates;
)V0G0N"
#else
R"V0G0N(
    in vec2 vPosition;
    in vec2 vTextureCoordinates;
)V0G0N"
#endif	
R"V0G0N(
    out vec2 fTextureCoordinates;

    void main ()
    {
        gl_Position = vec4(vPosition.x, vPosition.y, 0.0, 1.0);
        fTextureCoordinates = vTextureCoordinates;
    }
    
)V0G0N";

#endif
