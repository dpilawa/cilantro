#include "cilantroengine.h"
#include "MeshObject.h"
#include "GameObject.h"
#include "Material.h"
#include "Vector3f.h"
#include "Renderer.h"
#include "CallbackProvider.h"
#include "Mathf.h"
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
	faces.clear ();

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
		AddFace (i, i + 1, i + 2);
	};
	
	CalculateVertexNormals ();

	InvokeCallbacks ("OnUpdateMeshObject", this->GetHandle ());

	return *this;
}

__EAPI MeshObject & MeshObject::InitUnitSphere (unsigned int subdivisions, bool sharedVertices)
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
			AddFace (i, i + 1, i + 2);
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
				AddFace (i * lonSteps + j, i * lonSteps + (j + 1) % lonSteps, (i + 1) * lonSteps + j);
				AddFace (i * lonSteps + (j + 1) % lonSteps, (i + 1) * lonSteps + (j + 1) % lonSteps, (i + 1) * lonSteps + j);
			}
		}

		nIndex = (latSteps - 1) * lonSteps;
		sIndex = nIndex + 1;

		for (unsigned int j = 0; j <= lonSteps - 1; j++)
		{
			AddFace (nIndex, (j + 1) % lonSteps, j);
			AddFace (sIndex, nIndex - 1 - (j + 1) % lonSteps, nIndex - 1 - j);
		}

	}

	CalculateVertexNormals ();

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
		for (unsigned int f = 0; f < faces.size (); f += 3)
		{
			if ((faces[f] * 3 == v) || (faces[f + 1] * 3 == v) || (faces[f + 2] * 3 == v))
			{
				v1 = Vector3f (vertices[faces[f] * 3], vertices[faces[f] * 3 + 1], vertices[faces[f] * 3 + 2]);
				v2 = Vector3f (vertices[faces[f + 1] * 3], vertices[faces[f + 1] * 3 + 1], vertices[faces[f + 1] * 3 + 2]);
				v3 = Vector3f (vertices[faces[f + 2] * 3], vertices[faces[f + 2] * 3 + 1], vertices[faces[f + 2] * 3 + 2]);

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
	return (unsigned int) vertices.size ();
}

unsigned int MeshObject::GetFaceCount ()
{
	return (unsigned int) faces.size ();
}

void MeshObject::SetMaterial (Material & material)
{
	objectMaterial = &material;
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
	return faces.data ();
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

void MeshObject::AddVertex (const Vector3f & vertex)
{
	vertices.push_back (vertex[0]);
	vertices.push_back (vertex[1]);
	vertices.push_back (vertex[2]);
}

void MeshObject::AddFace (unsigned int v1, unsigned int v2, unsigned int v3)
{
	faces.push_back (v1);
	faces.push_back (v2);
	faces.push_back (v3);
}



