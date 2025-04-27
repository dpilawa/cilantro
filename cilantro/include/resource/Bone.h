#ifndef _BONE_H_
#define _BONE_H_

#include "cilantroengine.h"
#include "resource/Resource.h"
#include "math/Matrix4f.h"

namespace cilantro {

class __CEAPI Bone : public Resource, public std::enable_shared_from_this<Bone>
{
public:
    __EAPI Bone ();
    __EAPI virtual ~Bone ();

    __EAPI std::shared_ptr<Bone> SetOffsetMatrix (const Matrix4f& m);
    __EAPI Matrix4f GetOffsetMatrix () const;

private:
    Matrix4f m_offsetMatrix;
};

} // namespace cilantro

#endif