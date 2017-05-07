#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "Matrix4f.h"

class Transform
{
	float translateX, translateY, translateZ;
	float scaleX, scaleY, scaleZ;
	float rotateX, rotateY, rotateZ;
	Matrix4f transforMatrix;
public:
	Transform () : translateX (0.0f), translateY (0.0f), translateZ (0.0f),
		scaleX (1.0f), scaleY (1.0f), scaleZ (1.0f),
		rotateX (0.0f), rotateY (0.0f), rotateZ (0.0f) { };
	~Transform () { };
	Matrix4f GetMatrix () const;
	Matrix4f Translate (float x, float y, float z) const;
	Matrix4f Scale (float x, float y, float z) const;
	Matrix4f RotateX (float angle) const;
	Matrix4f RotateY (float angle) const;
	Matrix4f RotateZ (float angle) const;
private:
	void CalculateTransformMatrix ();
};

#endif