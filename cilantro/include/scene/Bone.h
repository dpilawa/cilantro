#ifndef _BONE_H_
#define _BONE_H_

#include "cilantroengine.h"
#include "scene/GameObject.h"
#include "math/Matrix4f.h"

class Bone : public GameObject
{
public:
    __EAPI Bone ();
    __EAPI virtual ~Bone ();

    __EAPI Bone& SetOffsetMatrix (const Matrix4f& m);
    __EAPI Matrix4f GetOffsetMatrix () const;

private:
    Matrix4f offsetMatrix;
};

#endif