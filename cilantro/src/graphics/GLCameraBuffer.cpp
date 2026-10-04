#include "graphics/GLCameraBuffer.h"
#include "graphics/GLUtils.h"
#include "scene/Camera.h"
#include "math/Mathf.h"
#include "math/Matrix4f.h"
#include <cstring>

namespace cilantro {

GLCameraBuffer::GLCameraBuffer ()
    : m_ubo (0)
    , m_matrices ()
{
}

GLCameraBuffer::~GLCameraBuffer ()
{
}

void GLCameraBuffer::Initialize ()
{
    // create uniform buffer for view and projection matrices
    glGenBuffers (1, &m_ubo);
    glBindBuffer (GL_UNIFORM_BUFFER, m_ubo);
    glBufferData (GL_UNIFORM_BUFFER, sizeof (SGlUniformMatrixBuffer), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase (GL_UNIFORM_BUFFER, static_cast<int>(EGlUBOType::UBO_MATRICES), m_ubo);

    GLUtils::CheckGLError (MSG_LOCATION);
}

void GLCameraBuffer::Deinitialize ()
{
    glDeleteBuffers (1, &m_ubo);
}

void GLCameraBuffer::Update (std::shared_ptr<Camera> camera, unsigned int width, unsigned int height)
{
    Matrix4f view = camera->GetViewMatrix ();
    Matrix4f projection = camera->GetProjectionMatrix (width, height);

    // load view matrix
    std::memcpy (m_matrices.viewMatrix, Mathf::Transpose (view)[0], 16 * sizeof (GLfloat));

    // load projection matrix
    std::memcpy (m_matrices.projectionMatrix, Mathf::Transpose (projection)[0], 16 * sizeof (GLfloat));

    // load to GPU - view and projection
    glBindBuffer (GL_UNIFORM_BUFFER, m_ubo);
    glBufferSubData (GL_UNIFORM_BUFFER, 0, 16 * sizeof (GLfloat), m_matrices.viewMatrix);
    glBufferSubData (GL_UNIFORM_BUFFER, 16 * sizeof (GLfloat), 16 * sizeof (GLfloat), m_matrices.projectionMatrix);
    glBindBuffer (GL_UNIFORM_BUFFER, 0);
}

} // namespace cilantro
