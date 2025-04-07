#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "cilantroengine.h"
#include "system/Hook.h"
#include "math/Vector3f.h"
#include "math/Matrix4f.h"
#include "math/Quaternion.h"
#include <string>
#include <memory>

namespace cilantro {

class __CEAPI Transform : public Hook<std::string>, public std::enable_shared_from_this<Transform>
{
public:
    __EAPI Transform ();
    __EAPI virtual ~Transform ();

    // returns models matrix (multiplication of scaling, rotation, translation)
    __EAPI Matrix4f GetTransformMatrix ();

    // sets transformation based on tranformation (model) matrix as input
    __EAPI std::shared_ptr<Transform> SetTransformMatrix (const Matrix4f& m);

    // returns transformation matrices
    __EAPI Matrix4f GetTranslationMatrix ();
    __EAPI Matrix4f GetScalingMatrix ();
    __EAPI Matrix4f GetRotationMatrix ();

    // translate
    __EAPI std::shared_ptr<Transform> Translate (float x, float y, float z);
    __EAPI std::shared_ptr<Transform> Translate (const Vector3f& t);
    __EAPI Vector3f GetTranslation () const;
    __EAPI std::shared_ptr<Transform> TranslateBy (float x, float y, float z);
    __EAPI std::shared_ptr<Transform> TranslateBy (const Vector3f& t);

    // scale
    __EAPI std::shared_ptr<Transform> Scale (float x, float y, float z);
    __EAPI std::shared_ptr<Transform> Scale (const Vector3f& s);
    __EAPI Vector3f GetScale () const;
    __EAPI std::shared_ptr<Transform> ScaleBy (float x, float y, float z);
    __EAPI std::shared_ptr<Transform> ScaleBy (const Vector3f& s);	

    // scale uniformly along all axes
    __EAPI std::shared_ptr<Transform> Scale (float s);
    __EAPI std::shared_ptr<Transform> ScaleBy (float s);

    // rotate (Euler angles in degrees)
    __EAPI std::shared_ptr<Transform> Rotate (float x, float y, float z);
    __EAPI std::shared_ptr<Transform> Rotate (const Vector3f& euler);
    __EAPI std::shared_ptr<Transform> Rotate (const Quaternion& q);
    __EAPI std::shared_ptr<Transform> Rotate (const Vector3f& axis, float theta);
    __EAPI Vector3f GetRotation () const;
    __EAPI Quaternion GetRotationQuaternion () const;
    __EAPI std::shared_ptr<Transform> RotateBy (float x, float y, float z);
    __EAPI std::shared_ptr<Transform> RotateBy (const Vector3f& r);
    __EAPI std::shared_ptr<Transform> RotateBy (const Quaternion& q);

private:

    bool m_isValid;

    Vector3f m_translate;
    Vector3f m_scale;
    Quaternion m_rotate;

    Matrix4f m_transformMatrix;

    Matrix4f m_scalingMatrix;
    Matrix4f m_translationMatrix;
    Matrix4f m_rotationMatrix;

};

} // namespace cilantro

#endif
