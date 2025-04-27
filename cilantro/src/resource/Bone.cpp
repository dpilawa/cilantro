#include "resource/Bone.h"

namespace cilantro {

Bone::Bone ()
{
}

Bone::~Bone ()
{
}

std::shared_ptr<Bone> Bone::SetOffsetMatrix (const Matrix4f& m)
{
    m_offsetMatrix = m;
    return std::dynamic_pointer_cast<Bone> (shared_from_this ());
}

Matrix4f Bone::GetOffsetMatrix () const
{
    return m_offsetMatrix;
}

} // namespace cilantro