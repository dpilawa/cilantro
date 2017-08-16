#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "GameObject.h"
#include "Vector3f.h"
#include "Matrix4f.h"
#include "Mathf.h"

class Camera : public GameObject
{
public:
	Camera (Vector3f& position, Vector3f& lookAt, Vector3f& up, float fov, float near, float far);
	~Camera ();

	Matrix4f GetViewMatrix () const;
	Matrix4f GetProjectionMatrix (unsigned int xRes, unsigned int yRes) const;

	// getters
	Vector3f GetPosition () const;

private:
	Vector3f cameraPosition;
	Vector3f cameraLookAt;
	Vector3f cameraUp;

	float cameraFOV;
	float nearPlane;
	float farPlane;
};

#endif

