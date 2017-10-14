#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "cilantroengine.h"
#include "Vector3f.h"
#include "GameObject.h"

class Camera : public GameObject
{
public:
	Camera (Vector3f& position, Vector3f& lookAt, Vector3f& up);
	~Camera ();

	Matrix4f GetViewMatrix () const;
	virtual Matrix4f GetProjectionMatrix (unsigned int xRes, unsigned int yRes) const = 0;

	// getters
	Vector3f GetPosition () const;

protected:

	Vector3f cameraPosition;
	Vector3f cameraLookAt;
	Vector3f cameraUp;

};

#endif

