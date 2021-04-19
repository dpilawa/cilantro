#include "graphics/GLDeferredRenderer.h"
#include "scene/MeshObject.h"

GLDeferredRenderer::GLDeferredRenderer (unsigned int width, unsigned int height) : GLRenderer (width, height)
{
#if (CILANTRO_GL_VERSION <= 140)
    framebuffer = new GLFramebuffer (width, height, 1, 0);
#else
    framebuffer = new GLMultisampleFramebuffer (width, height, 1, 0);
#endif

    gBuffer = new GLFramebuffer (width, height, 3, 1);
}

GLDeferredRenderer::~GLDeferredRenderer ()
{

}

void GLDeferredRenderer::Initialize ()
{
    // initialize framebuffer
    framebuffer->Initialize ();
    GLRenderer::Initialize ();
}

GLShaderProgram& GLDeferredRenderer::GetShaderMeshObjectProgram (const MeshObject& meshObject) 
{
    GLShaderProgram& shaderProgram = GetShaderProgramManager ().GetByName<GLShaderProgram> (meshObject.GetMaterial ().GetDeferredShaderProgramName ());

    return shaderProgram;
}