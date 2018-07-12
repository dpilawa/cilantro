#ifndef _PERSPECTIVECAMERA_H_
#define _PERSPECTIVECAMERA_H_

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

