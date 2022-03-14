#include "scene/Bone.h"

Bone::Bone (CGameScene* gameScene) : GameObject (gameScene)
{

}

Bone::~Bone ()
{

}

Bone& Bone::SetOffsetMatrix (const Matrix4f& m)
{
    offsetMatrix = m;

    return *this;
}

Matrix4f Bone::GetOffsetMatrix () const
{
    return offsetMatrix;
}