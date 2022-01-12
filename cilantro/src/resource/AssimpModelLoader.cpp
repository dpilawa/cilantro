#include "resource/AssimpModelLoader.h"
#include "system/LogMessage.h"
#include "system/Game.h"
#include "math/Mathf.h"
#include "resource/Mesh.h"
#include "resource/Texture.h"
#include "scene/AnimationObject.h"
#include "scene/GameObject.h"
#include "scene/Bone.h"
#include "scene/Material.h"
#include "scene/PhongMaterial.h"
#include "scene/PBRMaterial.h"
#include "math/Vector2f.h"
#include "math/Vector3f.h"
#include "math/Quaternion.h"
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
    const aiScene* scene = importer.ReadFile (path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_LimitBoneWeights);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        LogMessage(MSG_LOCATION, EXIT_FAILURE) << "Unable to load model" << path << importer.GetErrorString ();
    }
    else
    {
        ScanNode (scene, scene->mRootNode);
        ImportNode (scene, scene->mRootNode, nullptr);
        
        if (scene->HasAnimations ()) 
        {
            for (unsigned int i = 0; i < scene->mNumAnimations; i++)
            {
                ImportAnimation (scene->mAnimations[i]);
            }
        }

        LogMessage(MSG_LOCATION) << "Loaded model" << path << "meshes" << scene->mNumMeshes << "animations" << scene->mNumAnimations << "materials" << scene->mNumMaterials << "textures" << scene->mNumTextures << "lights" << scene->mNumLights;
    }
}

void AssimpModelLoader::ScanNode (const aiScene* scene, const aiNode* node)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        ScanMesh (scene->mMeshes[node->mMeshes[i]]);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ScanNode (scene, node->mChildren[i]);
    }
}

void AssimpModelLoader::ScanMesh (const aiMesh* mesh)
{
    for (unsigned int i = 0; i < mesh->mNumBones; i++)
    {
        boneNodes.insert (mesh->mBones[i]->mName.C_Str());
    }
}

void AssimpModelLoader::ImportNode (const aiScene* scene, const aiNode* node, const aiNode* parent)
{
    
    if (node->mNumMeshes == 0)
    {
        if (boneNodes.find (node->mName.C_Str()) != boneNodes.end ())
        {
            // create a simple Bone with transformation
            ImportBone (node, parent, node->mTransformation);
        }
        else
        {
            // create a simple hierarchy GameObject with transformation
            ImportGameObject (node, parent, node->mTransformation);
        }
    }
    else if (node->mNumMeshes == 1)
    {
        // create a MeshObject with node's transformation
        ImportMesh (scene, scene->mMeshes[node->mMeshes[0]], parent, node->mTransformation);
    }
    else 
    {
        // create a parent GameObject with transformation and import all child MeshObjects with identity transform
        ImportGameObject (node, parent, node->mTransformation);

        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            ImportMesh (scene, scene->mMeshes[node->mMeshes[i]], node, aiMatrix4x4 ());            
        }
    }

    // recursively import children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ImportNode (scene, node->mChildren[i], node);
    }

}

void AssimpModelLoader::ImportGameObject (const aiNode* node, const aiNode* parent, const aiMatrix4x4& transform)
{
    GameObject& gameObject = CreateGameObject (node, parent);
    gameObject.GetLocalTransform ().SetTransformMatrix (ConvertMatrix (transform));
}

void AssimpModelLoader::ImportBone (const aiNode* node, const aiNode* parent, const aiMatrix4x4& transform)
{
    Bone& bone = CreateBone (node, parent);
    bone.GetLocalTransform ().SetTransformMatrix (ConvertMatrix (transform));
    //bone.SetOffsetMatrix (Mathf::Invert (bone.GetModelTransformMatrix ()));
}

void AssimpModelLoader::ImportMesh (const aiScene* scene, const aiMesh* mesh, const aiNode* parent, const aiMatrix4x4& transform)
{
    Mesh& myMesh = Game::GetResourceManager ().Create<Mesh> (mesh->mName.C_Str ());

    ImportMeshPositions (myMesh, scene, mesh);
    ImportMeshFaces (myMesh, scene, mesh);
    ImportMeshBones (myMesh, scene, mesh);
    ImportMeshMaterial (myMesh, scene, mesh);

    MeshObject& meshObject = CreateMeshObject (myMesh, scene, mesh, parent);
    meshObject.GetLocalTransform ().SetTransformMatrix (ConvertMatrix (transform));
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
                myMesh.AddNormal (Vector3f (mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
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

void AssimpModelLoader::ImportMeshBones (Mesh& myMesh, const aiScene* scene, const aiMesh* mesh)
{
    if (mesh->HasBones ())
    {
        // check max number of bones (index 0 reserved for dummy identity "bone")
        if (mesh->mNumBones > CILANTRO_MAX_BONES - 1)
        {
            LogMessage(MSG_LOCATION, EXIT_FAILURE) << "Too many bones in mesh" << mesh->mName.C_Str ();
        }

        for (unsigned int i = 0; i < mesh->mNumBones; i++)
        {
            aiBone* bone = mesh->mBones[i];

            Bone& b = Game::GetGameScene ().GetGameObjectManager ().GetByName<Bone> (bone->mName.C_Str ());
            b.SetOffsetMatrix (ConvertMatrix (bone->mOffsetMatrix));

            for (unsigned j = 0; j < bone->mNumWeights; j++)
            {
                aiVertexWeight w = bone->mWeights[j];

                myMesh.AddVertexBoneInfluence (w.mVertexId, w.mWeight, b.GetHandle ());
            }

        }


    }
}

void AssimpModelLoader::ImportMeshMaterial (Mesh& myMesh, const aiScene* scene, const aiMesh* mesh)
{
    std::string textureTypes;

    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        // skip if material already loaded
        if (Game::GetGameScene ().GetMaterialManager ().HasName<Material> (material->GetName ().C_Str ()))
        {
            return;
        }

        for (unsigned int t = static_cast<unsigned int>(aiTextureType_NONE); t < AI_TEXTURE_TYPE_MAX; t++)
        {
            if (HasTexture (material, static_cast<aiTextureType>(t)))
            {
                textureTypes += (std::to_string (t) + " ");
            }
        }

        LogMessage (MSG_LOCATION) << "Texture types found:" << textureTypes;

        if (HasTexture (material, aiTextureType_DIFFUSE))
        {
            // phong material
            PhongMaterial& myMaterial = Game::GetGameScene ().AddMaterial<PhongMaterial> (material->GetName ().C_Str ());

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
            PBRMaterial& myMaterial = Game::GetGameScene ().AddMaterial<PBRMaterial> (material->GetName ().C_Str ());

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

void AssimpModelLoader::ImportAnimation (const aiAnimation* animation)
{
    AnimationObject& animationObject = CreateAnimationObject (animation);

    for (unsigned int i = 0; i < animation->mNumChannels; i++)
    {
        ImportNodeAnimation (animationObject, animation, animation->mChannels[i]);
    }
}

void AssimpModelLoader::ImportNodeAnimation (AnimationObject& animationObject, const aiAnimation* animation, const aiNodeAnim* nodeAnimation)
{
    std::string nodeName = nodeAnimation->mNodeName.C_Str ();
    GameObject& node = Game::GetGameScene ().GetGameObjectManager ().GetByName <GameObject> (nodeName);
    std::string propertyNameT = nodeName + "_T";
    std::string propertyNameR = nodeName + "_R";
    std::string propertyNameS = nodeName + "_S";
    
    for (unsigned int i = 0; i < nodeAnimation->mNumPositionKeys; i++)
    {
        if (i == 0)
        {
            // create animation property
            animationObject.AddAnimationProperty<Vector3f> (
                propertyNameT, 
                ConvertVector3f (nodeAnimation->mPositionKeys[i].mValue), 
                [&] (Vector3f v) {
                    node.GetLocalTransform ().Translate (v);
                },
                [] (Vector3f v0, Vector3f v1, float u) { 
                    return Mathf::Lerp (v0, v1, u); 
                }
            );
        }
        else 
        {
            // create keyframe
            animationObject.AddKeyframe<Vector3f> (propertyNameT, (float)(nodeAnimation->mPositionKeys[i].mTime / animation->mTicksPerSecond), ConvertVector3f (nodeAnimation->mPositionKeys[i].mValue));
        }
    }

    for (unsigned int i = 0; i < nodeAnimation->mNumRotationKeys; i++)
    {
        if (i == 0)
        {
            // create animation property
            animationObject.AddAnimationProperty<Quaternion> (
                propertyNameR, 
                ConvertQuaterion (nodeAnimation->mRotationKeys[i].mValue), 
                [&] (Quaternion q) {
                    node.GetLocalTransform ().Rotate (q);
                },
                [] (Quaternion q0, Quaternion q1, float u) { 
                    return Mathf::Slerp (q0, q1, u); 
                }
            );
        }
        else 
        {
            // create keyframe
            animationObject.AddKeyframe<Quaternion> (propertyNameR, (float)(nodeAnimation->mRotationKeys[i].mTime / animation->mTicksPerSecond), ConvertQuaterion (nodeAnimation->mRotationKeys[i].mValue));
        }
    }

    for (unsigned int i = 0; i < nodeAnimation->mNumScalingKeys; i++)
    {
        if (i == 0)
        {
            // create animation property
            animationObject.AddAnimationProperty<Vector3f> (
                propertyNameS, 
                ConvertVector3f (nodeAnimation->mScalingKeys[i].mValue), 
                [&] (Vector3f v) {
                    node.GetLocalTransform ().Scale (v);
                },
                [] (Vector3f v0, Vector3f v1, float u) { 
                    return Mathf::Lerp (v0, v1, u); 
                }
            );
        }
        else 
        {
            // create keyframe
            animationObject.AddKeyframe<Vector3f> (propertyNameS, (float)(nodeAnimation->mScalingKeys[i].mTime / animation->mTicksPerSecond), ConvertVector3f (nodeAnimation->mScalingKeys[i].mValue));
        }
    }

}

GameObject& AssimpModelLoader::CreateGameObject (const aiNode* node, const aiNode* parent)
{
    GameObject& gameObject = Game::GetGameScene ().AddGameObject<GameObject> (node->mName.C_Str ());
        
    if (parent != nullptr)
    {
        gameObject.SetParentObject (parent->mName.C_Str ());
    }
    
    return gameObject;
}

Bone& AssimpModelLoader::CreateBone (const aiNode* node, const aiNode* parent)
{
    Bone& bone = Game::GetGameScene ().AddGameObject<Bone> (node->mName.C_Str ());
        
    if (parent != nullptr)
    {
        bone.SetParentObject (parent->mName.C_Str ());
    }
    
    return bone;
}

MeshObject& AssimpModelLoader::CreateMeshObject (Mesh& myMesh, const aiScene* scene, const aiMesh* mesh, const aiNode* parent)
{
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    MeshObject& m = Game::GetGameScene ().AddGameObject<MeshObject> (myMesh.GetName (), myMesh.GetName (), material->GetName ().C_Str ());
        
    if (parent != nullptr)
    {
        m.SetParentObject (parent->mName.C_Str ());
    }
    
    return m;
}

AnimationObject& AssimpModelLoader::CreateAnimationObject (const aiAnimation* animation)
{
    AnimationObject& animationObject = Game::GetGameScene ().AddGameObject<AnimationObject> (animation->mName.C_Str ());

    return animationObject;
}

bool AssimpModelLoader::HasTexture (aiMaterial* material, aiTextureType type)
{
    return (material->GetTextureCount (type) > 0);
}

Texture& AssimpModelLoader::ImportMeshMaterialTexture (aiMaterial* material, aiTextureType type)
{
    aiString path;
    std::string sysPath;

    material->GetTexture (type, 0, &path);
    sysPath = std::string (path.C_Str ());

#if defined _WIN32 || defined __CYGWIN__
    std::replace(sysPath.begin(), sysPath.end(), '/', '\\');
#else
    std::replace(sysPath.begin(), sysPath.end(), '\\', '/');
#endif

    // if texture already exists, return it
    if (Game::GetResourceManager ().HasName<Texture> (sysPath))
    {
        return Game::GetResourceManager ().GetByName<Texture> (sysPath);
    }
    else 
    {
        if (material->GetTextureCount (type) > 1)
        {
            LogMessage (MSG_LOCATION, EXIT_FAILURE) << "Stacked textures not supported" << sysPath;
        }

        return Game::GetResourceManager ().Load<Texture> (sysPath, sysPath);

    }
}

Matrix4f AssimpModelLoader::ConvertMatrix (const aiMatrix4x4& m)
{
    return Matrix4f (Vector4f (m.a1, m.b1, m.c1, m.d1), Vector4f (m.a2, m.b2, m.c2, m.d2), Vector4f (m.a3, m.b3, m.c3, m.d3), Vector4f (m.a4, m.b4, m.c4, m.d4));
}

Vector3f AssimpModelLoader::ConvertVector3f (const aiVector3D& v)
{
    return Vector3f (v.x, v.y, v.z);
}

Quaternion AssimpModelLoader::ConvertQuaterion (const aiQuaternion& q)
{
    return Quaternion (q.w, q.x, q.y, q.z);
}