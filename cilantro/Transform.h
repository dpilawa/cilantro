#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "Matrix4f.h"

class Transform
{
	float translateX, translateY, translateZ;
	float scaleX, scaleY, scaleZ;
	float rotateX, rotateY, rotateZ;
	Matrix4f modelMatrix;
	Matrix4f scalingMatrix;
	Matrix4f translationMatrix;
	Matrix4f rotationXMatrix;
	Matrix4f rotationYMatrix;
	Matrix4f rotationZMatrix;
public:
	Transform () : translateX (0.0f), translateY (0.0f), translateZ (0.0f),
		scaleX (1.0f), scaleY (1.0f), scaleZ (1.0f),
		rotateX (0.0f), rotateY (0.0f), rotateZ (0.0f) { };
	~Transform () { };

	// returns models matrix (multiplication of rotation, scaling, translation)
	Matrix4f GetModelMatrix () const;
	// translate in space
	Transform& Translate (float x, float y, float z);
	// scale
	Transform& Scale (float x, float y, float z);
	// rotate in space around X axis
	Transform& RotateX (float angle);
	// rotate in space around Y axis	
	Transform& RotateY (float angle);
	// rotate in space around Z axis	
	Transform& RotateZ (float angle);
private:
	void CalculateModelMatrix ();
};

#endif