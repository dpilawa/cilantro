#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "Matrix4f.h"

class Transform
{

public:

	Transform ();
	~Transform ();

	// returns models matrix (multiplication of rotation, scaling, translation)
	Matrix4f& GetModelMatrix ();

	// returns transformation matrices
	Matrix4f& GetTranslationMatrix ();
	Matrix4f& GetScalingMatrix ();
	Matrix4f& GetRotationMatrix ();
	
	// translate
	Transform& Translate (float x, float y, float z);
	// scale
	Transform& Scale (float x, float y, float z);
	// rotate
	Transform& Rotate (float x, float y, float z);

private:

	bool hasChanged;

	float translateX, translateY, translateZ;
	float scaleX, scaleY, scaleZ;
	float rotateX, rotateY, rotateZ;

	Matrix4f modelMatrix;

	Matrix4f scalingMatrix;
	Matrix4f translationMatrix;
	Matrix4f rotationMatrix;

};

#endif