#include "cilantroengine.h"
#include "CallbackProvider.h"
#include "Transform.h"
#include "Matrix4f.h"
#include "Vector3f.h"
#include "Mathf.h"
#include <string>

Transform::Transform () : 
translateX (0.0f), translateY (0.0f), translateZ (0.0f),
scaleX (1.0f), scaleY (1.0f), scaleZ (1.0f),
rotateX (0.0f), rotateY (0.0f), rotateZ (0.0f)
{
	hasChanged = false;

	// calculate initial matrices
	Translate (0.0f, 0.0f, 0.0f);
	Scale (1.0f, 1.0f, 1.0f);
	Rotate (0.0f, 0.0f, 0.0f);

}

Transform::~Transform ()
{
}

Matrix4f& Transform::GetModelMatrix ()
{
	// multiply transformation matrices (scale, rotate, then translate)
	if (hasChanged == true) 
	{
		modelMatrix = translationMatrix * rotationMatrix * scalingMatrix;
		hasChanged = false;
	}

	return modelMatrix;
}

Matrix4f& Transform::GetTranslationMatrix () 
{
	return translationMatrix;
}

Matrix4f& Transform::GetScalingMatrix () 
{
	return scalingMatrix;
}

Matrix4f& Transform::GetRotationMatrix () 
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

	InvokeCallbacks ("OnUpdateTransform", 0u);

	return *this;
}

Transform& Transform::Translate (const Vector3f & t)
{
	return Translate (t[0], t[1], t[2]);
}

Transform& Transform::Scale (float x, float y, float z) 
{
	hasChanged = true;

	scaleX *= x;
	scaleY *= y;
	scaleZ *= z;

	scalingMatrix = Mathf::GenScalingMatrix (scaleX, scaleY, scaleZ);

	InvokeCallbacks ("OnUpdateTransform", 0u);

	return *this;
}

Transform& Transform::Scale (const Vector3f & s)
{
	return Scale (s[0], s[1], s[2]);
}

Transform& Transform::Scale (float s)
{
	return this->Scale (s, s, s);
}

Transform& Transform::Rotate (float x, float y, float z)
{
	hasChanged = true;

	rotateX += x;
	rotateY += y;
	rotateZ += z;

	rotationMatrix = Mathf::GenRotationXYZMatrix (Mathf::Deg2Rad (rotateX), Mathf::Deg2Rad (rotateY), Mathf::Deg2Rad (rotateZ));

	InvokeCallbacks ("OnUpdateTransform", 0u);

	return *this;
}

Transform& Transform::Rotate (const Vector3f & r)
{
	return Rotate (r[0], r[1], r[2]);
}



