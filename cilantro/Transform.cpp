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
	isValid = true;

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
	// multiply transformation matrices (first scale, then rotate, then translate)
	if (isValid == false) 
	{
		modelMatrix = translationMatrix * rotationMatrix * scalingMatrix;
		isValid = true;
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

Transform& Transform::SetTranslation (float x, float y, float z)
{
	isValid = false;

	translateX = x;
	translateY = y;
	translateZ = z;

	translationMatrix = Mathf::GenTranslationMatrix (translateX, translateY, translateZ);

	InvokeCallbacks ("OnUpdateTransform", 0u);

	return *this;
}

Transform& Transform::SetTranslation (const Vector3f & t)
{
	return SetTranslation (t[0], t[1], t[2]);
}

Transform& Transform::Translate (float x, float y, float z)
{
	return SetTranslation (translateX + x, translateY + y, translateZ + z);
}

Transform& Transform::Translate (const Vector3f & t)
{
	return SetTranslation (translateX + t[0], translateY + t[1], translateZ + t[2]);
}

Transform& Transform::SetScaling (float x, float y, float z) 
{
	isValid = false;

	scaleX = x;
	scaleY = y;
	scaleZ = z;

	scalingMatrix = Mathf::GenScalingMatrix (scaleX, scaleY, scaleZ);

	InvokeCallbacks ("OnUpdateTransform", 0u);

	return *this;
}

Transform& Transform::SetScaling (const Vector3f & s)
{
	return SetScaling (s[0], s[1], s[2]);
}

Transform& Transform::Scale (float x, float y, float z) 
{
	return SetScaling (scaleX * x, scaleY * y, scaleZ * z);
}

Transform& Transform::Scale (const Vector3f & s)
{
	return SetScaling (scaleX * s[0], scaleY * s[1], scaleZ * s[2]);
}

Transform& Transform::SetScaling (float s)
{
	return SetScaling (s, s, s);
}

Transform& Transform::Scale (float s)
{
	return Scale (s, s, s);
}

Transform& Transform::SetRotation (float x, float y, float z)
{
	isValid = false;

	rotateX = x;
	rotateY = y;
	rotateZ = z;

	rotationMatrix = Mathf::GenRotationXYZMatrix (Mathf::Deg2Rad (rotateX), Mathf::Deg2Rad (rotateY), Mathf::Deg2Rad (rotateZ));

	InvokeCallbacks ("OnUpdateTransform", 0u);

	return *this;
}

Transform& Transform::SetRotation (const Vector3f & r)
{
	return SetRotation (r[0], r[1], r[2]);
}

Transform& Transform::Rotate (float x, float y, float z)
{
	return SetRotation (rotateX + x, rotateY + y, rotateZ + z);
}

Transform& Transform::Rotate (const Vector3f & r)
{
	return SetRotation (rotateX + r[0], rotateY + r[1], rotateZ + r[2]);
}



