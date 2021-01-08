#ifndef _EMISSIVE_FS_H_
#define _EMISSIVE_FS_H_

#include <string>

#define xstr(a) str(a)
#define str(a) #a

std::string gEmissiveFragmentShader = 
R"(#version )"
xstr(CILANTRO_GL_VERSION)
R"V0G0N(

    /* texture uv coordinates */
    in vec2 fUV;

)V0G0N"
#if (CILANTRO_GL_VERSION >= 420)
R"V0G0N(

    /* material properties */
    layout (binding = 3) uniform sampler2D tEmissive;

)V0G0N"
#else
R"V0G0N(

    uniform sampler2D tEmissive;

)V0G0N"
#endif	
R"V0G0N(

    vec3 fEmissiveColor;

    /* output color */
    out vec4 color;

    void main()
    {
        fEmissiveColor = texture (tEmissive, fUV).rgb;
        color = vec4 (fEmissiveColor, 1.0);
    } 
    
)V0G0N";

#endif