#include "resource/AssimpModelLoader.h"
#include "system/LogMessage.h"
#include "system/EngineContext.h"
#include "resource/Mesh.h"
#include "math/Vector2f.h"
#include "math/Vector3f.h"
#include <assimp/postprocess.h>

AssimpModelLoader::AssimpModelLoader ()
{

}

AssimpModelLoader::~AssimpModelLoader ()
{
    
}

void AssimpModelLoader::Load (std::string path)
{
    const aiScene* scene = importer.ReadFile (path, 
            aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        LogMessage(MSG_LOCATION, EXIT_FAILURE) << "Unable to load model" << path << importer.GetErrorString ();
    }
    else
    {
        ImportNode (scene, scene->mRootNode);
        LogMessage(MSG_LOCATION) << "Loaded model" << path << "meshes" << scene->mNumMeshes << "materials" << scene->mNumMaterials << "textures" << scene->mNumTextures << "lights" << scene->mNumLights;
    }
}

void AssimpModelLoader::ImportNode (const aiScene* scene, const aiNode* node)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        ImportMesh (scene, scene->mMeshes[node->mMeshes[i]]);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ImportNode (scene, node->mChildren[i]);
    }

}

void AssimpModelLoader::ImportMesh (const aiScene* scene, const aiMesh* mesh)
{
    Mesh& myMesh = EngineContext::GetResourceManager ().Create<Mesh> (mesh->mName.C_Str ());

    if (mesh->HasPositions ())
    {
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            /* vertices with UV coordinates */
            if (!mesh->HasTextureCoords (0))
            {
                myMesh.AddVertex (Vector3f (mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z), Vector2f (mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y));
            }
            else
            {
                myMesh.AddVertex (Vector3f (mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z), Vector2f ());
            }

            /* normals */
            if (mesh->HasNormals ())
            {
                myMesh.AddNormal (Vector3f (mesh->mNormals[i].x, mesh->mNormals[i].x, mesh->mNormals[i].z));
            }

            /* tangents and bitangents */
            if (mesh->HasTangentsAndBitangents ())
            {
                myMesh.AddTangentBitangent (Vector3f (mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z), Vector3f (mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z));
            }

        }
    }

    if (mesh->HasFaces ())
    {
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            if (mesh->mFaces[i].mNumIndices != 3)
            {
                LogMessage(MSG_LOCATION, EXIT_FAILURE) << "Unsupported number of face indices:" << mesh->mFaces[i].mNumIndices;
            }
            else {
                myMesh.AddFace (mesh->mFaces[i].mIndices[0], mesh->mFaces[i].mIndices[1], mesh->mFaces[i].mIndices[2]);
            }
        }
    }

}