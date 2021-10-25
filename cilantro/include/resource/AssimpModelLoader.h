#ifndef _ASSIMPMODELLOADER_H_
#define _ASSIMPMODELLOADER_H_

#include "cilantroengine.h"
#include "resource/ModelLoader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <string>
#include <set>

class GameObject;
class Mesh;
class MeshObject;
class Bone;
class Texture;
class Matrix4f;

class AssimpModelLoader : public ModelLoader
{
public:
    __EAPI AssimpModelLoader ();
    __EAPI virtual ~AssimpModelLoader ();

    __EAPI void Load (std::string path);

private:

    Assimp::Importer importer;
    std::set<std::string> boneNodes;

    void ScanNode (const aiScene* scene, const aiNode* node);
    void ScanMesh (const aiMesh* mesh);

    void ImportNode (const aiScene* scene, const aiNode* node, const aiNode* parent);
    void ImportGameObject (const aiNode* node, const aiNode* parent, const aiMatrix4x4& transform);
    void ImportBone (const aiNode* node, const aiNode* parent, const aiMatrix4x4& transform);
    void ImportMesh (const aiScene* scene, const aiMesh* mesh, const aiNode* parent, const aiMatrix4x4& transform);

    void ImportMeshPositions (Mesh& myMesh, const aiScene* scene, const aiMesh* mesh);
    void ImportMeshFaces (Mesh& myMesh, const aiScene* scene, const aiMesh* mesh);
    void ImportMeshBones (Mesh& myMesh, const aiScene* scene, const aiMesh* mesh);
    void ImportMeshMaterial (Mesh& myMesh, const aiScene* scene, const aiMesh* mesh);

    GameObject& CreateGameObject (const aiNode* node, const aiNode* parent);
    Bone& CreateBone (const aiNode* node, const aiNode* parent);
    MeshObject& CreateMeshObject (Mesh& myMesh, const aiScene* scene, const aiMesh* mesh, const aiNode* parent);

    bool HasTexture (aiMaterial* material, aiTextureType type);
    Texture& ImportMeshMaterialTexture (aiMaterial* material, aiTextureType type);

    Matrix4f ConvertMatrix (const aiMatrix4x4& m);
};

#endif