#include "cilantroengine.h"

class RotatingCube : public MeshObject
{
public:
	RotatingCube ();
	~RotatingCube ();

	void OnFrame ();
private:
	// rotation speed in degrees per second
	float rotationSpeed;
};

