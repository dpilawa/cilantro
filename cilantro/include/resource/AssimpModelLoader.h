#ifndef _ASSIMPMODELLOADER_H_
#define _ASSIMPMODELLOADER_H_

#include "cilantroengine.h"
#include "system/Game.h"
#include "resource/ModelLoader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <string>
#include <set>
#include <memory>

namespace cilantro {

class GameScene;
class GameObject;
class AnimationObject;
class Mesh;
class MeshObject;
class Bone;
class Texture;
class Vector3f;
class Matrix4f;
class Quaternion;

class __CEAPI AssimpModelLoader : public ModelLoader
{
public:
    __EAPI AssimpModelLoader (std::shared_ptr<Game> game);
    __EAPI virtual ~AssimpModelLoader ();

    __EAPI void Load (std::string sceneName, std::string path);

private:
    std::shared_ptr<Game> m_game;
    std::shared_ptr<GameScene> m_gameScene;

    Assimp::Importer m_importer;
    std::set<std::string> m_boneNodes;

    void ScanNode (const aiScene* scene, const aiNode* node);
    void ScanMesh (const aiMesh* mesh);

    void ImportNode (const aiScene* scene, const aiNode* node, const aiNode* parent);
    void ImportGameObject (const aiNode* node, const aiNode* parent, const aiMatrix4x4& transform);
    void ImportBone (const aiNode* node, const aiNode* parent, const aiMatrix4x4& transform);
    void ImportMesh (const aiScene* scene, const aiMesh* mesh, const aiNode* parent, const aiMatrix4x4& transform);

    void ImportMeshPositions (std::shared_ptr<Mesh> myMesh, const aiScene* scene, const aiMesh* mesh);
    void ImportMeshFaces (std::shared_ptr<Mesh> myMesh, const aiScene* scene, const aiMesh* mesh);
    void ImportMeshBones (std::shared_ptr<Mesh> myMesh, const aiScene* scene, const aiMesh* mesh);
    void ImportMeshMaterial (std::shared_ptr<Mesh> myMesh, const aiScene* scene, const aiMesh* mesh);

    void ImportAnimation (const aiAnimation* animation);
    void ImportNodeAnimation (std::shared_ptr<AnimationObject> myAnimationObject, const aiAnimation* animation, const aiNodeAnim* nodeAnimation);

    std::shared_ptr<GameObject> CreateGameObject (const aiNode* node, const aiNode* parent);
    std::shared_ptr<Bone> CreateBone (const aiNode* node, const aiNode* parent);
    std::shared_ptr<MeshObject> CreateMeshObject (std::shared_ptr<Mesh> myMesh, const aiScene* scene, const aiMesh* mesh, const aiNode* parent);
    std::shared_ptr<AnimationObject> CreateAnimationObject (const aiAnimation* animation);

    bool HasTexture (aiMaterial* material, aiTextureType type);
    std::shared_ptr<Texture> ImportMeshMaterialTexture (aiMaterial* material, aiTextureType type);

    Matrix4f ConvertMatrix (const aiMatrix4x4& m);
    Vector3f ConvertVector3f (const aiVector3D& v);
    Quaternion ConvertQuaterion (const aiQuaternion& q);
};

} // namespace cilantro

#endif