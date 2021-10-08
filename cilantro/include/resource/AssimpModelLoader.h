#ifndef _ASSIMPMODELLOADER_H_
#define _ASSIMPMODELLOADER_H_

#include "cilantroengine.h"
#include "resource/ModelLoader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <string>

class Mesh;
class MeshObject;
class Texture;

class AssimpModelLoader : public ModelLoader
{
public:
    __EAPI AssimpModelLoader ();
    __EAPI virtual ~AssimpModelLoader ();

    __EAPI void Load (std::string path);

private:

    Assimp::Importer importer;

    void ImportNode (const aiScene* scene, const aiNode* node, const aiNode* parent);
    void ImportMesh (const aiScene* scene, const aiMesh* mesh, const aiMesh* parent, const aiMatrix4x4& t);

    void ImportMeshPositions (Mesh& myMesh, const aiScene* scene, const aiMesh* mesh);
    void ImportMeshFaces (Mesh& myMesh, const aiScene* scene, const aiMesh* mesh);
    void ImportMeshMaterial (Mesh& myMesh, const aiScene* scene, const aiMesh* mesh);

    MeshObject& CreateMeshObject (Mesh& myMesh, const aiScene* scene, const aiMesh* mesh, const aiMesh* parent);

    bool HasTexture (aiMaterial* material, aiTextureType type);
    Texture& ImportMeshMaterialTexture (aiMaterial* material, aiTextureType type);
};

#endif