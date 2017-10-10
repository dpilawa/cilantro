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

	virtual Matrix4f GetViewMatrix () const = 0;
	virtual Matrix4f GetProjectionMatrix (unsigned int xRes, unsigned int yRes) const = 0;

	// getters
	virtual Vector3f GetPosition () const = 0;

protected:

	Vector3f cameraPosition;
	Vector3f cameraLookAt;
	Vector3f cameraUp;

};

#endif

