#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "cilantroengine.h"
#include "Matrix4f.h"
#include "Mathf.h"

class Transform
{
public:

	__EAPI Transform ();
	__EAPI ~Transform ();

	// returns models matrix (multiplication of rotation, scaling, translation)
	__EAPI Matrix4f& GetModelMatrix ();

	// returns transformation matrices
	__EAPI Matrix4f& GetTranslationMatrix ();
	__EAPI Matrix4f& GetScalingMatrix ();
	__EAPI Matrix4f& GetRotationMatrix ();
	
	// translate
	__EAPI Transform& Translate (float x, float y, float z);
	// scale
	__EAPI Transform& Scale (float x, float y, float z);
	// rotate (angles in degrees)
	__EAPI Transform& Rotate (float x, float y, float z);

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