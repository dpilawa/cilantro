#include "cilantroengine.h"
#include "scene/MeshObject.h"
#include "scene/GameObject.h"
#include "scene/Material.h"
#include "math/Vector3f.h"
#include "math/Mathf.h"
#include "graphics/Renderer.h"
#include "util/CallbackProvider.h"

#include <vector>

MeshObject::MeshObject ()
{
}

MeshObject::~MeshObject ()
{
}

MeshObject& MeshObject::Clear ()
{
    vertices.clear ();
    normals.clear ();
    indices.clear ();

    InvokeCallbacks ("OnUpdateMeshObject", this->GetHandle ());

    return *this;
}

void MeshObject::CalculateVertexNormals ()
{
    Vector3f normal;
    Vector3f v1, v2, v3;

    normals.clear ();

    // loop through all vertices
    for (unsigned int v = 0; v < vertices.size (); v += 3)
    {
        normal = Vector3f (0.0f, 0.0f, 0.0f);

        // loop through all faces and if face contains vertex, add face normal to vertex normal
        for (unsigned int f = 0; f < indices.size (); f += 3)
        {
            if ((indices[f] * 3 == v) || (indices[f + 1] * 3 == v) || (indices[f + 2] * 3 == v))
            {
                v1 = Vector3f (vertices[indices[f] * 3], vertices[indices[f] * 3 + 1], vertices[indices[f] * 3 + 2]);
                v2 = Vector3f (vertices[indices[f + 1] * 3], vertices[indices[f + 1] * 3 + 1], vertices[indices[f + 1] * 3 + 2]);
                v3 = Vector3f (vertices[indices[f + 2] * 3], vertices[indices[f + 2] * 3 + 1], vertices[indices[f + 2] * 3 + 2]);

                normal += Mathf::Cross (v2 - v1, v3 - v1);
            }
        }

        // normalize
        normal = Mathf::Normalize (normal);

        // store normal
        normals.push_back (normal[0]);
        normals.push_back (normal[1]);
        normals.push_back (normal[2]);
    }
}

unsigned int MeshObject::GetVertexCount ()
{
    return (unsigned int) (vertices.size () / 3);
}

unsigned int MeshObject::GetIndexCount ()
{
    return (unsigned int) indices.size ();
}

MeshObject& MeshObject::SetMaterial (Material & material)
{
    objectMaterial = &material;

    return *this;
}

Material & MeshObject::GetMaterial () const
{
    return *objectMaterial;
}

float* MeshObject::GetVerticesData ()
{
    return vertices.data ();
}

float* MeshObject::GetNormalsData ()
{
    return normals.data ();
}

unsigned int* MeshObject::GetFacesData ()
{
    return indices.data ();
}

void MeshObject::AddVertex (const Vector3f& vertex)
{
    vertices.push_back (vertex[0]);
    vertices.push_back (vertex[1]);
    vertices.push_back (vertex[2]);
}

void MeshObject::AddFace (unsigned int v1, unsigned int v2, unsigned int v3)
{
    indices.push_back (v1);
    indices.push_back (v2);
    indices.push_back (v3);
}

void MeshObject::OnFrame ()
{
    GameObject::OnFrame ();
}

void MeshObject::OnDraw (Renderer& renderer)
{
    GameObject::OnDraw (renderer);
    renderer.Draw (*this);
}


void MeshObject::OnUpdate (Renderer& renderer)
{
    GameObject::OnUpdate (renderer);
    renderer.Update (*this);
}




