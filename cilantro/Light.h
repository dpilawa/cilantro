#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "cilantroengine.h"
#include "GameObject.h"

class Light : public GameObject
{
public:
	Light ();
	virtual ~Light ();

	virtual void SetEnabled (bool value);
	
	__EAPI bool IsEnabled () const;

private:

	bool isEnabled;
};

#endif

