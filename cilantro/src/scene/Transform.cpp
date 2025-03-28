#include "cilantroengine.h"
#include "system/HookProvider.h"
#include "scene/Transform.h"
#include "math/Matrix4f.h"
#include "math/Vector3f.h"
#include "math/Mathf.h"

#include <string>

namespace cilantro
{

Transform::Transform ()
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

Matrix4f& Transform::GetTransformMatrix ()
{
    // multiply transformation matrices (first scale, then rotate, then translate)
    if (isValid == false) 
    {
        transformMatrix = translationMatrix * rotationMatrix * scalingMatrix;
        isValid = true;
    }

    return transformMatrix;
}

Transform& Transform::SetTransformMatrix (const Matrix4f& m)
{
    isValid = true; // OK because model matrix is calculated

    transformMatrix = m;

    translate = Mathf::GetTranslationFromTransformationMatrix (m);
    translationMatrix = Mathf::GenTranslationMatrix (translate);

    scale = Mathf::GetScalingFromTransformationMatrix (m);
    scalingMatrix = Mathf::GenScalingMatrix (scale);

    rotate = Mathf::GetRotationFromTransformationMatrix (m);
    rotationMatrix = Mathf::GenRotationMatrix (rotate);

    InvokeHook ("OnUpdateTransform");

    return *this;
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
    return Translate (Vector3f (x, y, z));
}

Transform& Transform::Translate (const Vector3f & t)
{
    isValid = false;

    translate = t;
    translationMatrix = Mathf::GenTranslationMatrix (translate);

    InvokeHook ("OnUpdateTransform");

    return *this;
}

Vector3f Transform::GetTranslation () const
{
    Vector3f t;

    t[0] = translationMatrix[0][3];
    t[1] = translationMatrix[1][3];
    t[2] = translationMatrix[2][3];

    return t;
}

Transform& Transform::TranslateBy (float x, float y, float z)
{
    return TranslateBy (Vector3f (x, y, z));
}

Transform& Transform::TranslateBy (const Vector3f& t) 
{
    return Translate (GetTranslation () + t);
}

Transform& Transform::Scale (float x, float y, float z) 
{
    return Scale (Vector3f (x, y, z));
}

Transform& Transform::Scale (const Vector3f& s)
{
    isValid = false;

    scale = s;
    scalingMatrix = Mathf::GenScalingMatrix (scale);

    InvokeHook ("OnUpdateTransform");

    return *this;
}

Vector3f Transform::GetScale () const
{
    Vector3f s;

    s[0] = scalingMatrix[0][0];
    s[1] = scalingMatrix[1][1];
    s[2] = scalingMatrix[2][2];

    return s;
}

Transform& Transform::ScaleBy (float x, float y, float z)
{
    return ScaleBy (Vector3f (x, y, z));
}

Transform& Transform::ScaleBy (const Vector3f& s) 
{
    Vector3f sNew;

    sNew = GetScale ();

    sNew[0] = sNew[0] * s[0];
    sNew[0] = sNew[0] * s[0];
    sNew[0] = sNew[0] * s[0];

    return Scale (sNew);
}

Transform& Transform::Scale (float s)
{
    return Scale (Vector3f (s, s, s));
}

Transform& Transform::ScaleBy (float s)
{
    return Scale (GetScale () * s);
}

Transform& Transform::Rotate (float x, float y, float z)
{
    return Rotate (Vector3f (x, y, z));
}

Transform& Transform::Rotate (const Vector3f& euler)
{
    isValid = false;

    rotate = Mathf::EulerToQuaternion (Mathf::Deg2Rad (euler));
    rotationMatrix = Mathf::GenRotationMatrix (rotate);

    InvokeHook ("OnUpdateTransform");

    return *this;
}

Transform& Transform::Rotate (const Quaternion& q)
{
    isValid = false;

    rotate = q;
    rotationMatrix = Mathf::GenRotationMatrix (rotate);

    InvokeHook ("OnUpdateTransform");

    return *this;
}

Transform& Transform::Rotate (const Vector3f& axis, float theta)
{
    isValid = false;

    rotate = Mathf::GenRotationQuaternion (axis, Mathf::Deg2Rad (theta));
    rotationMatrix = Mathf::GenRotationMatrix (rotate);

    InvokeHook ("OnUpdateTransform");

    return *this;
}

Vector3f Transform::GetRotation () const
{
    return (Mathf::Rad2Deg (Mathf::QuaternionToEuler (rotate)));
}

Quaternion Transform::GetRotationQuaternion () const
{
    return rotate;
}

Transform& Transform::RotateBy (float x, float y, float z)
{
    return RotateBy (Vector3f (x, y, z));
}

Transform& Transform::RotateBy (const Vector3f& r) 
{
    return Rotate (GetRotation () + r);
}

Transform& Transform::RotateBy (const Quaternion& q)
{
    Quaternion newRotation;

    newRotation = Mathf::Product (q, rotate);

    return Rotate (newRotation);
}

} // namespace cilantro