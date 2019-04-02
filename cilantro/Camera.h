#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "cilantroengine.h"
#include "GameObject.h"
#include "Matrix4f.h"

class Camera : public GameObject
{
public:
	Camera ();
	virtual ~Camera ();

	Matrix4f GetViewMatrix ();
	virtual Matrix4f GetProjectionMatrix (unsigned int xRes, unsigned int yRes) const = 0;
};

#endif

