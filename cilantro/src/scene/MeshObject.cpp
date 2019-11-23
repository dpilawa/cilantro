#include "cilantroengine.h"
#include "scene/MeshObject.h"
#include "scene/GameObject.h"
#include "scene/Material.h"
#include "math/Vector3f.h"
#include "math/Mathf.h"
#include "graphics/Renderer.h"
#include "util/CallbackProvider.h"

#include <vector>

// template instantiations
template __EAPI MeshObject& MeshObject::InitCurve (const Curve<Vector3f, 2>& curve, unsigned int samples);
template __EAPI MeshObject& MeshObject::InitCurve (const Curve<Vector3f, 3>& curve, unsigned int samples);

MeshObject::MeshObject (MeshType type)
{
    meshType = type;
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


MeshObject& MeshObject::InitUnitCube ()
{
	Clear ();

	vertices = std::vector<float> {

		-0.5f, -0.5f,  0.5f, 
		 0.5f, -0.5f,  0.5f, 
		-0.5f,  0.5f,  0.5f, 
		-0.5f,  0.5f,  0.5f, 
		 0.5f, -0.5f,  0.5f, 
		 0.5f,  0.5f,  0.5f, 

		-0.5f,  0.5f,  0.5f, 
		 0.5f,  0.5f,  0.5f, 
		-0.5f,  0.5f, -0.5f, 
		-0.5f,  0.5f, -0.5f, 
		 0.5f,  0.5f,  0.5f, 
		 0.5f,  0.5f, -0.5f, 

		-0.5f,  0.5f, -0.5f, 
		 0.5f,  0.5f, -0.5f, 
		-0.5f, -0.5f, -0.5f, 
		-0.5f, -0.5f, -0.5f, 
		 0.5f,  0.5f, -0.5f, 
		 0.5f, -0.5f, -0.5f, 

		-0.5f, -0.5f, -0.5f, 
		 0.5f, -0.5f, -0.5f, 
		-0.5f, -0.5f,  0.5f, 
		-0.5f, -0.5f,  0.5f, 
		 0.5f, -0.5f, -0.5f, 
		 0.5f, -0.5f,  0.5f, 

		 0.5f, -0.5f,  0.5f, 
		 0.5f, -0.5f, -0.5f, 
		 0.5f,  0.5f,  0.5f, 
		 0.5f,  0.5f,  0.5f, 
		 0.5f, -0.5f, -0.5f, 
		 0.5f,  0.5f, -0.5f, 

		-0.5f, -0.5f, -0.5f, 
		-0.5f, -0.5f,  0.5f, 
		-0.5f,  0.5f, -0.5f, 
		-0.5f,  0.5f, -0.5f, 
		-0.5f, -0.5f,  0.5f, 
		-0.5f,  0.5f,  0.5f

	};

	for (unsigned int i = 0; i < 36; i += 3)
	{
		AddFace (i, i + 1, i + 2, false);
	};
	
	CalculateVertexNormals ();

	InvokeCallbacks ("OnUpdateMeshObject", this->GetHandle ());

	return *this;
}

MeshObject& MeshObject::InitUnitSphere (unsigned int subdivisions, bool sharedVertices)
{
	float step;
	float theta, phi;
	unsigned int lonSteps, latSteps;
	unsigned int nIndex, sIndex;

	Clear ();

	step = Mathf::Pi () / ((subdivisions + 1) * 2);
	lonSteps = (subdivisions + 1) * 4;
	latSteps = (subdivisions + 1) * 2;

	if (sharedVertices == false)
	{
		// generate vertices
		for (unsigned int i = 1; i < latSteps - 1; i++)
		{
			theta = step * i;
			
			for (unsigned int j = 0; j <= lonSteps - 1; j++)
			{
				phi = step * j;

				AddVertex (Mathf::Spherical2Cartesian (theta, phi, 1.0f));
				AddVertex (Mathf::Spherical2Cartesian (theta, phi + step, 1.0f));
				AddVertex (Mathf::Spherical2Cartesian (theta + step, phi, 1.0f));

				AddVertex (Mathf::Spherical2Cartesian (theta, phi + step, 1.0f));
				AddVertex (Mathf::Spherical2Cartesian (theta + step, phi + step, 1.0f));
				AddVertex (Mathf::Spherical2Cartesian (theta + step, phi, 1.0f));
			}
		}

		theta = 0.0f;
		for (unsigned int j = 0; j <= lonSteps - 1; j++)
		{
			phi = step * j;

			AddVertex (Mathf::Spherical2Cartesian (theta + step, phi, 1.0f));
			AddVertex (Mathf::Spherical2Cartesian (theta, phi, 1.0f));
			AddVertex (Mathf::Spherical2Cartesian (theta + step, phi + step, 1.0f));
        }

		theta = Mathf::Pi ();
		for (unsigned int j = 0; j <= lonSteps - 1; j++)
		{
			phi = step * j;

			AddVertex (Mathf::Spherical2Cartesian (theta, phi, 1.0f));
			AddVertex (Mathf::Spherical2Cartesian (theta - step, phi, 1.0f));
			AddVertex (Mathf::Spherical2Cartesian (theta - step, phi + step, 1.0f));
		}

		// generate faces
		for (unsigned int i = 0; i < latSteps * lonSteps * 3 * 2; i += 3)
		{
			AddFace (i, i + 1, i + 2, false);
		}

	}
	else
	{
		// generate sphere vertices
		for (unsigned int i = 1; i <= latSteps - 1; i++)
		{
			theta = step * i;
	
			for (unsigned int j = 0; j <= lonSteps - 1; j++)
			{
				phi = step * j;
				AddVertex (Mathf::Spherical2Cartesian (theta, phi, 1.0f));
			}
		}

		// add poles
		AddVertex (Vector3f (0.0f, 1.0f, 0.0f));
		AddVertex (Vector3f (0.0f, -1.0f, 0.0f));

		// generate face data
		for (unsigned int i = 0; i < latSteps - 2; i++)
		{
			for (unsigned int j = 0; j <= lonSteps - 1; j++)
			{
				AddFace (i * lonSteps + j, i * lonSteps + (j + 1) % lonSteps, (i + 1) * lonSteps + j, false);
				AddFace (i * lonSteps + (j + 1) % lonSteps, (i + 1) * lonSteps + (j + 1) % lonSteps, (i + 1) * lonSteps + j, false);
			}
		}

		nIndex = (latSteps - 1) * lonSteps;
		sIndex = nIndex + 1;

		for (unsigned int j = 0; j <= lonSteps - 1; j++)
		{
			AddFace (nIndex, (j + 1) % lonSteps, j, false);
			AddFace (sIndex, nIndex - 1 - (j + 1) % lonSteps, nIndex - 1 - j, false);
		}

	}

	CalculateVertexNormals ();

	InvokeCallbacks ("OnUpdateMeshObject", this->GetHandle ());

	return *this;
}

template <int d>
MeshObject& MeshObject::InitCurve (const Curve<Vector3f, d>& curve, unsigned int samples)
{
    Clear ();
    
    for (unsigned int i = 0; i <= samples; i++)
    {
        float t = static_cast<float>(i) / static_cast<float>(samples);
        Vector3f p = curve.GetCurvePoint (t);
		
        AddVertex (p);
		if (i > 0)
		{
            AddLine (i - 1, i);
        }
    }

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

MeshType MeshObject::GetMeshType ()
{
    return meshType;
}

unsigned int MeshObject::AddVertex (const Vector3f & vertex)
{
	vertices.push_back (vertex[0]);
	vertices.push_back (vertex[1]);
	vertices.push_back (vertex[2]);

    return GetVertexCount () - 1;
}

void MeshObject::AddPoint (unsigned int v, bool renrererUpdate)
{
	indices.push_back (v);
	
	if (renrererUpdate)
	{
		InvokeCallbacks ("OnUpdateMeshObject", this->GetHandle ());
	}
}

void MeshObject::AddLine (unsigned int v1, unsigned int v2, bool renrererUpdate)
{
	indices.push_back (v1);
	indices.push_back (v2);

	if (renrererUpdate)
	{
		InvokeCallbacks ("OnUpdateMeshObject", this->GetHandle ());
	}
}

void MeshObject::AddFace (unsigned int v1, unsigned int v2, unsigned int v3, bool renrererUpdate)
{
	indices.push_back (v1);
	indices.push_back (v2);
	indices.push_back (v3);

	if (renrererUpdate)
	{
		InvokeCallbacks ("OnUpdateMeshObject", this->GetHandle ());
	}
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




