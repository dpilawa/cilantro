#ifndef _NORMALS_FS_H_
#define _NORMALS_FS_H_

#include <string>

#define xstr(a) str(a)
#define str(a) #a

std::string gNormalsFragmentShader = 
R"(#version )"
xstr(CILANTRO_MIN_GL_VERSION)
R"V0G0N(

    #define MAX_LIGHTS 100

    /* fragment position in world space */
    in vec3 fPosition;

    /* fragment normal */
    in vec3 fNormal;

    /* output color */
    out vec4 color;

    void main()
    {
        color = vec4 (0.5 + 0.5 * fNormal, 1.0);
    } 
    
)V0G0N";

#endif
