#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "cilantroengine.h"
#include "GameObject.h"

class Light : public GameObject
{
public:
	Light ();
	virtual ~Light ();

	virtual void Enable () = 0;
	virtual void Disable () = 0;

protected:

	bool isEnabled;
};

#endif

