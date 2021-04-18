#include "graphics/GLForwardRenderer.h"
#include "scene/MeshObject.h"

GLForwardRenderer::GLForwardRenderer (unsigned int width, unsigned int height) : GLRenderer (width, height)
{
#if (CILANTRO_GL_VERSION <= 140)
    framebuffer = new GLFramebuffer (width, height);
#else
    framebuffer = new GLMultisampleFramebuffer (width, height);
#endif
}

GLForwardRenderer::~GLForwardRenderer ()
{

}

void GLForwardRenderer::Initialize ()
{
    // initialize framebuffer
    framebuffer->Initialize ();

    GLRenderer::Initialize ();
}

GLShaderProgram& GLForwardRenderer::GetShaderMeshObjectProgram (const MeshObject& meshObject) 
{
    GLShaderProgram& shaderProgram = GetShaderProgramManager ().GetByName<GLShaderProgram> (meshObject.GetMaterial ().GetForwardShaderProgramName ());

    return shaderProgram;
}
