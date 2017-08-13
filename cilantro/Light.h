#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "Vector3f.h"
#include "Vector4f.h"
#include "GameObject.h"

class Light : public GameObject
{
public:
	Light ();
	~Light ();

	virtual void Enable () = 0;
	virtual void Disable () = 0;

	Vector4f GetPosition ();

protected:
	Vector4f lightPosition;
	bool isEnabled;
};

#endif

