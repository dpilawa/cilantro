#include "Transform.h"
#include "Mathf.h"
#include "Matrix4f.h"

Transform& Transform::Scale (float x, float y, float z) 
{
	scaleX *= x;
	scaleY *= y;
	scaleZ *= z;

	return *this;
}

void Transform::CalculateModelMatrix ()
{
	scalingMatrix = Mathf::GenScalingMatrix (scaleX, scaleY, scaleZ);
	translationMatrix = Mathf::GenTranslationMatrix (translateX, translateY, translateZ);

	modelMatrix = translationMatrix * scalingMatrix;
}

Matrix4f Transform::GetModelMatrix () const
{
	return modelMatrix;
}

Transform& Transform::Translate (float x, float y, float z)
{
	translateX += x;
	translateY += y;
	translateZ += z;

	return *this;
}