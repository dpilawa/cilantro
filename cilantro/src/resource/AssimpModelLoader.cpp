#include "resource/AssimpModelLoader.h"
#include "system/LogMessage.h"
#include "system/EngineContext.h"
#include "resource/Mesh.h"
#include "resource/Texture.h"
#include "scene/PhongMaterial.h"
#include "scene/PBRMaterial.h"
#include "math/Vector2f.h"
#include "math/Vector3f.h"
#include <assimp/postprocess.h>
#include <string>

AssimpModelLoader::AssimpModelLoader ()
{

}

AssimpModelLoader::~AssimpModelLoader ()
{
    
}

void AssimpModelLoader::Load (std::string path)
{
    const aiScene* scene = importer.ReadFile (path, 
            aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace);

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

    ImportMeshPositions (myMesh, scene, mesh);
    ImportMeshFaces (myMesh, scene, mesh);
    ImportMeshMaterial (myMesh, scene, mesh);
}

void AssimpModelLoader::ImportMeshPositions (Mesh& myMesh, const aiScene* scene, const aiMesh* mesh)
{
    if (mesh->HasPositions ())
    {
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            /* vertices with UV coordinates */
            if (mesh->HasTextureCoords (0))
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
}

void AssimpModelLoader::ImportMeshFaces (Mesh& myMesh, const aiScene* scene, const aiMesh* mesh)
{
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

void AssimpModelLoader::ImportMeshMaterial (Mesh& myMesh, const aiScene* scene, const aiMesh* mesh)
{
    std::string textureName;

    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        if (HasTexture (material, aiTextureType_DIFFUSE))
        {
            // phong material
            PhongMaterial& myMaterial = EngineContext::GetGameScene ().AddMaterial<PhongMaterial> (material->GetName ().C_Str ());

            if (HasTexture (material, aiTextureType_DIFFUSE))
            {
                myMaterial.SetDiffuse (ImportMeshMaterialTexture (material, aiTextureType_DIFFUSE).GetName ());
            }

            if (HasTexture (material, aiTextureType_NORMALS))
            {
                myMaterial.SetNormal (ImportMeshMaterialTexture (material, aiTextureType_NORMALS).GetName ());
            }

            if (HasTexture (material, aiTextureType_SPECULAR))
            {
                myMaterial.SetSpecular (ImportMeshMaterialTexture (material, aiTextureType_SPECULAR).GetName ());
            }

            if (HasTexture (material, aiTextureType_EMISSIVE))
            {
                myMaterial.SetEmissive (ImportMeshMaterialTexture (material, aiTextureType_EMISSIVE).GetName ());
            }

        }
        else if (HasTexture (material, aiTextureType_BASE_COLOR))
        {
            // PBR material
            PBRMaterial& myMaterial = EngineContext::GetGameScene ().AddMaterial<PBRMaterial> (material->GetName ().C_Str ());

            if (HasTexture (material, aiTextureType_BASE_COLOR))
            {
                myMaterial.SetAlbedo (ImportMeshMaterialTexture (material, aiTextureType_BASE_COLOR).GetName ());
            }

            if (HasTexture (material, aiTextureType_NORMAL_CAMERA))
            {
                myMaterial.SetNormal (ImportMeshMaterialTexture (material, aiTextureType_NORMAL_CAMERA).GetName ());
            }

            if (HasTexture (material, aiTextureType_METALNESS))
            {
                myMaterial.SetMetallic (ImportMeshMaterialTexture (material, aiTextureType_METALNESS).GetName ());
            }

            if (HasTexture (material, aiTextureType_DIFFUSE_ROUGHNESS))
            {
                myMaterial.SetRoughness (ImportMeshMaterialTexture (material, aiTextureType_DIFFUSE_ROUGHNESS).GetName ());
            }

            if (HasTexture (material, aiTextureType_AMBIENT_OCCLUSION))
            {
                myMaterial.SetAO (ImportMeshMaterialTexture (material, aiTextureType_AMBIENT_OCCLUSION).GetName ());
            }

        }   
    }
}

bool AssimpModelLoader::HasTexture (aiMaterial* material, aiTextureType type)
{
    return (material->GetTextureCount (type) > 0);
}

Texture& AssimpModelLoader::ImportMeshMaterialTexture (aiMaterial* material, aiTextureType type)
{
    aiString path;
    std::string textureName;
    std::string sysPath;

    textureName = std::string (material->GetName ().C_Str ());
    textureName += std::to_string (type);

    if (material->GetTextureCount (type) > 1)
    {
        LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Stacked textures not supported" << textureName;
    }
    else
    {
        material->GetTexture (type, 0, &path);
        sysPath = sysPath = std::string (path.C_Str ());

#if defined _WIN32 || defined __CYGWIN__
        std::replace(sysPath.begin(), sysPath.end(), '/', '\\');
#else
        std::replace(sysPath.begin(), sysPath.end(), '\\', '/');
#endif

    }

    return EngineContext::GetResourceManager ().Load<Texture> (textureName, sysPath);
}