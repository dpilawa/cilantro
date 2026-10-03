#pragma once

#include "cilantroengine.h"
#include "graphics/GLTypes.h"
#include <memory>

namespace cilantro {

class Camera;

// uniform buffer with view and projection matrices of the active camera
class GLCameraBuffer
{
public:
    GLCameraBuffer ();
    virtual ~GLCameraBuffer ();

    // create and bind GL buffer
    void Initialize ();
    void Deinitialize ();

    // load view and projection matrices of the camera to GPU
    void Update (std::shared_ptr<Camera> camera, unsigned int width, unsigned int height);

private:
    GLuint m_ubo;
    SGlUniformMatrixBuffer m_matrices;
};

} // namespace cilantro
