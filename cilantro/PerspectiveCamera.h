#ifndef _PERSPECTIVECAMERA_H_
#define _PERSPECTIVECAMERA_H_

#include "cilantroengine.h"
#include "Vector3f.h"
#include "Matrix4f.h"
#include "Mathf.h"
#include "Camera.h"

class PerspectiveCamera : public Camera
{
public:
	__EAPI PerspectiveCamera (float fov, float near, float far);
	__EAPI ~PerspectiveCamera ();

	__EAPI Matrix4f GetProjectionMatrix (unsigned int xRes, unsigned int yRes) const;

private:

	float cameraFOV;
	float nearPlane;
	float farPlane;
};

#endif

