#ifndef _ORTHOGRAPHICCAMERA_H_
#define _ORTHOGRAPHICCAMERA_H_

#include "cilantroengine.h"
#include "scene/Camera.h"
#include "math/Matrix4f.h"

class OrthographicCamera : public Camera
{
public:
	__EAPI OrthographicCamera (float width, float near, float far);
	__EAPI ~OrthographicCamera ();

	__EAPI Matrix4f GetProjectionMatrix (unsigned int xRes, unsigned int yRes) const;

private:

	float cameraWidth;
	float nearPlane;
	float farPlane;
};

#endif