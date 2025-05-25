#ifndef __GLUTILS_H__
#define __GLUTILS_H__

#include "cilantroengine.h"
#include "glad/gl.h"
#include "system/LogMessage.h"
#include <string>

namespace cilantro {

struct GLSLVersionInfo {
    int versionNumber;     // e.g. 300, 330
    std::string directive; // e.g. "#version 300 es"
};

class __CEAPI GLUtils
{
public:
    __EAPI static void PrintGLInfo ();
    __EAPI static void PrintGLExtensions ();

    // check for OpenGL errors
    __EAPI static void CheckGLError (const std::string& location);

    // get GLSL version
    __EAPI static GLSLVersionInfo GetGLSLVersion ();

private:
    __EAPI static GLSLVersionInfo m_glslVersionInfo;
    __EAPI static bool m_hasGLSLVersionInfo;

};


}

#endif