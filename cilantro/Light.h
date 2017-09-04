#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "GameObject.h"

class Light : public GameObject
{
public:
	Light ();
	~Light ();

	virtual void Enable () = 0;
	virtual void Disable () = 0;

protected:
	
	bool isEnabled;
};

#endif

