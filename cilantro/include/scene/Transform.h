#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "cilantroengine.h"
#include "system/HookProvider.h"
#include "math/Vector3f.h"
#include "math/Matrix4f.h"
#include "math/Quaternion.h"
#include <string>

class Transform : public Hook<std::string>
{
public:

    __EAPI Transform ();
    __EAPI ~Transform ();

    // returns models matrix (multiplication of scaling, rotation, translation)
    __EAPI Matrix4f& GetTransformMatrix ();

    // sets transformation based on tranformation (model) matrix as input
    __EAPI Transform& SetTransformMatrix (const Matrix4f& m);

    // returns transformation matrices
    __EAPI Matrix4f& GetTranslationMatrix ();
    __EAPI Matrix4f& GetScalingMatrix ();
    __EAPI Matrix4f& GetRotationMatrix ();

    // translate
    __EAPI Transform& Translate (float x, float y, float z);
    __EAPI Transform& Translate (const Vector3f& t);
    __EAPI Vector3f GetTranslation () const;
    __EAPI Transform& TranslateBy (float x, float y, float z);
    __EAPI Transform& TranslateBy (const Vector3f& t);

    // scale
    __EAPI Transform& Scale (float x, float y, float z);
    __EAPI Transform& Scale (const Vector3f& s);
    __EAPI Vector3f GetScale () const;
    __EAPI Transform& ScaleBy (float x, float y, float z);
    __EAPI Transform& ScaleBy (const Vector3f& s);	

    // scale uniformly along all axes
    __EAPI Transform& Scale (float s);
    __EAPI Transform& ScaleBy (float s);

    // rotate (Euler angles in degrees)
    __EAPI Transform& Rotate (float x, float y, float z);
    __EAPI Transform& Rotate (const Vector3f& euler);
    __EAPI Transform& Rotate (const Quaternion& q);
    __EAPI Transform& Rotate (const Vector3f& axis, float theta);
    __EAPI Vector3f GetRotation () const;
    __EAPI Quaternion GetRotationQuaternion () const;
    __EAPI Transform& RotateBy (float x, float y, float z);
    __EAPI Transform& RotateBy (const Vector3f& r);
    __EAPI Transform& RotateBy (const Quaternion& q);

private:

    bool isValid;

    Vector3f translate;
    Vector3f scale;
    Quaternion rotate;

    Matrix4f transformMatrix;

    Matrix4f scalingMatrix;
    Matrix4f translationMatrix;
    Matrix4f rotationMatrix;

};

#endif
