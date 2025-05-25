#include "graphics/GLUtils.h"
#include "system/LogMessage.h"
#include <string>
#include <sstream>

namespace cilantro {

__EAPI bool GLUtils::m_hasGLSLVersionInfo = false;
__EAPI GLSLVersionInfo GLUtils::m_glslVersionInfo = {0, ""};

void GLUtils::PrintGLInfo ()
{
    GLint data;
    const GLubyte* renderer = glGetString (GL_RENDERER);
    const GLubyte* version = glGetString (GL_VERSION);
    const GLubyte* shadingLanguageVersion = glGetString (GL_SHADING_LANGUAGE_VERSION);

    LogMessage () << "OpenGL Renderer: " << renderer;
    LogMessage () << "OpenGL Version: " << version;
    LogMessage () << "OpenGL Shading Language Version: " << shadingLanguageVersion;

    glGetIntegerv (GL_MAX_GEOMETRY_SHADER_INVOCATIONS, &data);
    if (glGetError() == GL_NO_ERROR)
    {
        LogMessage () << "GL_MAX_GEOMETRY_SHADER_INVOCATIONS =" << std::to_string (data);
    }

    glGetIntegerv (GL_MAX_ARRAY_TEXTURE_LAYERS, &data);
    if (glGetError() == GL_NO_ERROR)
    {
        LogMessage () << "GL_MAX_ARRAY_TEXTURE_LAYERS =" << std::to_string (data);
    }

    glGetIntegerv (GL_MAX_VERTEX_OUTPUT_COMPONENTS, &data);
    if (glGetError() == GL_NO_ERROR)
    {
        LogMessage () << "GL_MAX_VERTEX_OUTPUT_COMPONENTS =" << std::to_string (data);
    }

    glGetIntegerv (GL_MAX_FRAMEBUFFER_LAYERS, &data);
    if (glGetError() == GL_NO_ERROR)
    {
        LogMessage () << "GL_MAX_FRAMEBUFFER_LAYERS =" << std::to_string (data); 
    }

    glGetIntegerv (GL_MAX_FRAMEBUFFER_WIDTH, &data);
    if (glGetError() == GL_NO_ERROR)
    {
        LogMessage () << "GL_MAX_FRAMEBUFFER_WIDTH =" << std::to_string (data); 
    }

    glGetIntegerv (GL_MAX_FRAMEBUFFER_HEIGHT, &data);
    if (glGetError() == GL_NO_ERROR)
    {
        LogMessage () << "GL_MAX_FRAMEBUFFER_HEIGHT =" << std::to_string (data); 
    }

    glGetIntegerv (GL_MAX_UNIFORM_BLOCK_SIZE, &data);
    if (glGetError() == GL_NO_ERROR)
    {
        LogMessage () << "GL_MAX_UNIFORM_BLOCK_SIZE =" << std::to_string (data); 
    }

}

void GLUtils::PrintGLExtensions ()
{
    GLint numExtensions;
    glGetIntegerv (GL_NUM_EXTENSIONS, &numExtensions);

    LogMessage () << "OpenGL Extensions:";

    for (GLint i = 0; i < numExtensions; ++i)
    {
        LogMessage () << "  " << glGetStringi (GL_EXTENSIONS, i);
    }
}

void GLUtils::CheckGLError (const std::string& location)
{
    GLuint errorCode;

    if ((errorCode = glGetError ()) != GL_NO_ERROR)
    {
        LogMessage (location, EXIT_FAILURE) << "glError:" << std::hex << std::showbase << errorCode;
    }

}

GLSLVersionInfo GLUtils::GetGLSLVersion ()
{
    if (m_hasGLSLVersionInfo)
    {
        return m_glslVersionInfo;
    }
    else {
        GLSLVersionInfo info;
        bool isES = false;

        const GLubyte* verStr = glGetString(GL_SHADING_LANGUAGE_VERSION);

        std::string version(reinterpret_cast<const char*>(verStr));
        std::stringstream ss;

        // check for "ES" in the version string
        isES = (version.find("ES") != std::string::npos || version.find("es") != std::string::npos);

        // Extract numeric part (e.g., "3.00", "3.10", etc.)
        size_t pos = version.find_first_of("0123456789");
        if (pos != std::string::npos) {
            ss.str(version.substr(pos));
            int major = 0;
            int minor = 0;
            ss >> major;
            if (ss.peek() == '.') ss.ignore();
            ss >> minor;

            info.versionNumber = major * 100 + minor;

            // compose GLSL #version directive
            info.directive = std::to_string(info.versionNumber);
            if (isES && info.versionNumber >= 300) {
                info.directive += " es";
            }
        }

        return info;
    }
}

} // namespace cilantro