#ifndef _CAMERA_H_
#define _CAMERA_H_

class Camera : public GameObject
{
public:
	Camera ();
	~Camera ();

	Matrix4f GetViewMatrix ();
	virtual Matrix4f GetProjectionMatrix (unsigned int xRes, unsigned int yRes) const = 0;
};

#endif

