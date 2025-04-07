#include "cilantroengine.h"
#include "system/Hook.h"
#include "scene/Transform.h"
#include "math/Matrix4f.h"
#include "math/Vector3f.h"
#include "math/Mathf.h"

#include <string>

namespace cilantro
{

Transform::Transform ()
{
    m_isValid = true;

    // calculate initial matrices
    m_translate = Vector3f (0.0f, 0.0f, 0.0f);
    m_translationMatrix = Mathf::GenTranslationMatrix (m_translate);

    m_scale = Vector3f (1.0f, 1.0f, 1.0f);
    m_scalingMatrix = Mathf::GenScalingMatrix (m_scale);
    
    m_rotate = Mathf::EulerToQuaternion (Mathf::Deg2Rad ({0.0f, 0.0f, 0.0f}));
    m_rotationMatrix = Mathf::GenRotationMatrix (m_rotate);

    m_transformMatrix = m_translationMatrix * m_rotationMatrix * m_scalingMatrix;
}

Transform::~Transform ()
{
}

Matrix4f Transform::GetTransformMatrix ()
{
    // multiply transformation matrices (first scale, then rotate, then translate)
    if (m_isValid == false) 
    {
        m_transformMatrix = m_translationMatrix * m_rotationMatrix * m_scalingMatrix;
        m_isValid = true;
    }

    return m_transformMatrix;
}

std::shared_ptr<Transform> Transform::SetTransformMatrix (const Matrix4f& m)
{
    m_isValid = true; // OK because model matrix is calculated

    m_transformMatrix = m;

    m_translate = Mathf::GetTranslationFromTransformationMatrix (m);
    m_translationMatrix = Mathf::GenTranslationMatrix (m_translate);

    m_scale = Mathf::GetScalingFromTransformationMatrix (m);
    m_scalingMatrix = Mathf::GenScalingMatrix (m_scale);

    m_rotate = Mathf::GetRotationFromTransformationMatrix (m);
    m_rotationMatrix = Mathf::GenRotationMatrix (m_rotate);

    InvokeHook ("OnUpdateTransform");

    return std::dynamic_pointer_cast<Transform> (shared_from_this ());
}

Matrix4f Transform::GetTranslationMatrix ()
{
    return m_translationMatrix;
}

Matrix4f Transform::GetScalingMatrix ()
{
    return m_scalingMatrix;
}

Matrix4f Transform::GetRotationMatrix ()
{
    return m_rotationMatrix;
}

std::shared_ptr<Transform> Transform::Translate (float x, float y, float z)
{
    return Translate (Vector3f (x, y, z));
}

std::shared_ptr<Transform> Transform::Translate (const Vector3f & t)
{
    m_isValid = false;

    m_translate = t;
    m_translationMatrix = Mathf::GenTranslationMatrix (m_translate);

    InvokeHook ("OnUpdateTransform");

    return std::dynamic_pointer_cast<Transform> (shared_from_this ());
}

Vector3f Transform::GetTranslation () const
{
    Vector3f t;

    t[0] = m_translationMatrix[0][3];
    t[1] = m_translationMatrix[1][3];
    t[2] = m_translationMatrix[2][3];

    return t;
}

std::shared_ptr<Transform> Transform::TranslateBy (float x, float y, float z)
{
    return TranslateBy (Vector3f (x, y, z));
}

std::shared_ptr<Transform> Transform::TranslateBy (const Vector3f& t) 
{
    return Translate (GetTranslation () + t);
}

std::shared_ptr<Transform> Transform::Scale (float x, float y, float z) 
{
    return Scale (Vector3f (x, y, z));
}

std::shared_ptr<Transform> Transform::Scale (const Vector3f& s)
{
    m_isValid = false;

    m_scale = s;
    m_scalingMatrix = Mathf::GenScalingMatrix (m_scale);

    InvokeHook ("OnUpdateTransform");

    return std::dynamic_pointer_cast<Transform> (shared_from_this ());
}

Vector3f Transform::GetScale () const
{
    Vector3f s;

    s[0] = m_scalingMatrix[0][0];
    s[1] = m_scalingMatrix[1][1];
    s[2] = m_scalingMatrix[2][2];

    return s;
}

std::shared_ptr<Transform> Transform::ScaleBy (float x, float y, float z)
{
    return ScaleBy (Vector3f (x, y, z));
}

std::shared_ptr<Transform> Transform::ScaleBy (const Vector3f& s) 
{
    Vector3f sNew;

    sNew = GetScale ();

    sNew[0] = sNew[0] * s[0];
    sNew[0] = sNew[0] * s[0];
    sNew[0] = sNew[0] * s[0];

    return Scale (sNew);
}

std::shared_ptr<Transform> Transform::Scale (float s)
{
    return Scale (Vector3f (s, s, s));
}

std::shared_ptr<Transform> Transform::ScaleBy (float s)
{
    return Scale (GetScale () * s);
}

std::shared_ptr<Transform> Transform::Rotate (float x, float y, float z)
{
    return Rotate (Vector3f (x, y, z));
}

std::shared_ptr<Transform> Transform::Rotate (const Vector3f& euler)
{
    m_isValid = false;

    m_rotate = Mathf::EulerToQuaternion (Mathf::Deg2Rad (euler));
    m_rotationMatrix = Mathf::GenRotationMatrix (m_rotate);

    InvokeHook ("OnUpdateTransform");

    return std::dynamic_pointer_cast<Transform> (shared_from_this ());
}

std::shared_ptr<Transform> Transform::Rotate (const Quaternion& q)
{
    m_isValid = false;

    m_rotate = q;
    m_rotationMatrix = Mathf::GenRotationMatrix (m_rotate);

    InvokeHook ("OnUpdateTransform");

    return std::dynamic_pointer_cast<Transform> (shared_from_this ());
}

std::shared_ptr<Transform> Transform::Rotate (const Vector3f& axis, float theta)
{
    m_isValid = false;

    m_rotate = Mathf::GenRotationQuaternion (axis, Mathf::Deg2Rad (theta));
    m_rotationMatrix = Mathf::GenRotationMatrix (m_rotate);

    InvokeHook ("OnUpdateTransform");

    return std::dynamic_pointer_cast<Transform> (shared_from_this ());
}

Vector3f Transform::GetRotation () const
{
    return (Mathf::Rad2Deg (Mathf::QuaternionToEuler (m_rotate)));
}

Quaternion Transform::GetRotationQuaternion () const
{
    return m_rotate;
}

std::shared_ptr<Transform> Transform::RotateBy (float x, float y, float z)
{
    return RotateBy (Vector3f (x, y, z));
}

std::shared_ptr<Transform> Transform::RotateBy (const Vector3f& r) 
{
    return Rotate (GetRotation () + r);
}

std::shared_ptr<Transform> Transform::RotateBy (const Quaternion& q)
{
    Quaternion newRotation;

    newRotation = Mathf::Product (q, m_rotate);

    return Rotate (newRotation);
}

} // namespace cilantro