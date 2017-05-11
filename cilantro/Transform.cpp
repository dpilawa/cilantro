#include "Transform.h"
#include "Mathf.h"

Transform& Transform::Scale (float x, float y, float z) 
{
	scaleX *= x;
	scaleY *= y;
	scaleZ *= z;

	scalingMatrix = Mathf::GenScalingMatrix (scaleX, scaleY, scaleZ);
	CalculateModelMatrix ();

	return *this;
}

Transform& Transform::Translate (float x, float y, float z)
{
	translateX += x;
	translateY += y;
	translateZ += z;

	translationMatrix = Mathf::GenTranslationMatrix (translateX, translateY, translateZ);
	CalculateModelMatrix ();

	return *this;
}