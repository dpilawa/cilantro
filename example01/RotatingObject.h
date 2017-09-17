#include "cilantroengine.h"

class RotatingObject : public MeshObject
{
public:
	RotatingObject ();
	~RotatingObject ();

	void OnFrame ();
private:
	// rotation speed in degrees per second
	float rotationSpeed;
};

