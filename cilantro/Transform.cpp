#include "Transform.h"
#include "Mathf.h"
#include "Matrix4f.h"

Transform::Transform () : 
translateX (0.0f), translateY (0.0f), translateZ (0.0f),
scaleX (1.0f), scaleY (1.0f), scaleZ (1.0f),
rotateX (0.0f), rotateY (0.0f), rotateZ (0.0f)
{
	hasChanged = false;
}

Transform::~Transform ()
{
}

Matrix4f Transform::GetModelMatrix ()
{
	// multiply transformation matrices (rotate, then scale, then translate)
	if (hasChanged == true) 
	{
		modelMatrix = translationMatrix * scalingMatrix * rotationMatrix;
		hasChanged = false;
	}

	return modelMatrix;
}

Matrix4f Transform::GetTranslationMatrix () const
{
	return translationMatrix;
}

Matrix4f Transform::GetScalingMatrix () const
{
	return scalingMatrix;
}

Matrix4f Transform::GetRotationMatrix () const
{
	return rotationMatrix;
}

Transform& Transform::Translate (float x, float y, float z)
{
	hasChanged = true;

	translateX += x;
	translateY += y;
	translateZ += z;

	translationMatrix = Mathf::GenTranslationMatrix (translateX, translateY, translateZ);

	return *this;
}

Transform& Transform::Scale (float x, float y, float z) 
{
	hasChanged = true;

	scaleX *= x;
	scaleY *= y;
	scaleZ *= z;

	scalingMatrix = Mathf::GenScalingMatrix (scaleX, scaleY, scaleZ);

	return *this;
}

Transform & Transform::Rotate (float x, float y, float z)
{
	hasChanged = true;

	rotateX += x;
	rotateY += y;
	rotateZ += z;

	rotationMatrix = Mathf::GenRotationXYZMatrix (rotateX, rotateY, rotateZ);

	return *this;
}



