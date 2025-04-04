#ifndef _BONE_H_
#define _BONE_H_

#include "cilantroengine.h"
#include "scene/GameObject.h"
#include "math/Matrix4f.h"

namespace cilantro {

class __CEAPI Bone : public GameObject
{
public:
    __EAPI Bone (std::shared_ptr<GameScene> gameScene);
    __EAPI virtual ~Bone ();

    __EAPI Bone& SetOffsetMatrix (const Matrix4f& m);
    __EAPI Matrix4f GetOffsetMatrix () const;

private:
    Matrix4f m_offsetMatrix;
};

} // namespace cilantro

#endif