#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "cilantroengine.h"
#include "CallbackProvider.h"
#include "Matrix4f.h"

class Transform : public CallbackProvider<std::string>
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
	__EAPI Transform& Translate (const Vector3f& t);
	// scale
	__EAPI Transform& Scale (float x, float y, float z);
	__EAPI Transform& Scale (const Vector3f& s);
	// scale along all axes
	__EAPI Transform& Scale (float s);
	// rotate (angles in degrees)
	__EAPI Transform& Rotate (float x, float y, float z);
	__EAPI Transform& Rotate (const Vector3f& r);

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