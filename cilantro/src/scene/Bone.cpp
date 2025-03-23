#include "scene/Bone.h"

namespace cilantro {

Bone::Bone (std::shared_ptr<GameScene> gameScene) : GameObject (gameScene)
{
}

Bone::~Bone ()
{
}

Bone& Bone::SetOffsetMatrix (const Matrix4f& m)
{
    m_offsetMatrix = m;
    return *this;
}

Matrix4f Bone::GetOffsetMatrix () const
{
    return m_offsetMatrix;
}

} // namespace cilantro