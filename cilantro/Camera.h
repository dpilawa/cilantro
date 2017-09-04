#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "cilantroengine.h"
#include "Vector3f.h"
#include "Matrix4f.h"
#include "Mathf.h"
#include "GameObject.h"

class Camera : public GameObject
{
public:
	__EAPI Camera (Vector3f& position, Vector3f& lookAt, Vector3f& up, float fov, float near, float far);
	__EAPI ~Camera ();

	__EAPI Matrix4f GetViewMatrix () const;
	__EAPI Matrix4f GetProjectionMatrix (unsigned int xRes, unsigned int yRes) const;

	// getters
	__EAPI Vector3f GetPosition () const;

private:
	Vector3f cameraPosition;
	Vector3f cameraLookAt;
	Vector3f cameraUp;

	float cameraFOV;
	float nearPlane;
	float farPlane;
};

#endif

