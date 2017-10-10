#include "Camera.h"

Camera::Camera (Vector3f& position, Vector3f& lookAt, Vector3f& up) :
cameraPosition (position), cameraLookAt (lookAt), cameraUp (up)
{
}

Camera::~Camera ()
{
}
