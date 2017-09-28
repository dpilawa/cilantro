#include "MeshObject.h"

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

	for (unsigned int i = 0; i < 36; ++i)
	{
		faces.push_back (i);
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
	Vector3f cartesian;

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

				cartesian = Mathf::Spherical2Cartesian (theta, phi, 1.0f);
				vertices.push_back (cartesian.GetX ());
				vertices.push_back (cartesian.GetY ());
				vertices.push_back (cartesian.GetZ ());

				cartesian = Mathf::Spherical2Cartesian (theta, phi + step, 1.0f);
				vertices.push_back (cartesian.GetX ());
				vertices.push_back (cartesian.GetY ());
				vertices.push_back (cartesian.GetZ ());

				cartesian = Mathf::Spherical2Cartesian (theta + step, phi, 1.0f);
				vertices.push_back (cartesian.GetX ());
				vertices.push_back (cartesian.GetY ());
				vertices.push_back (cartesian.GetZ ());

				cartesian = Mathf::Spherical2Cartesian (theta, phi + step, 1.0f);
				vertices.push_back (cartesian.GetX ());
				vertices.push_back (cartesian.GetY ());
				vertices.push_back (cartesian.GetZ ());

				cartesian = Mathf::Spherical2Cartesian (theta + step, phi + step, 1.0f);
				vertices.push_back (cartesian.GetX ());
				vertices.push_back (cartesian.GetY ());
				vertices.push_back (cartesian.GetZ ());

				cartesian = Mathf::Spherical2Cartesian (theta + step, phi, 1.0f);
				vertices.push_back (cartesian.GetX ());
				vertices.push_back (cartesian.GetY ());
				vertices.push_back (cartesian.GetZ ());

			}
		}

		// generate faces
		for (unsigned int i = 0; i < latSteps * lonSteps * 3 * 2; i++)
		{
			faces.push_back (i);
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
				cartesian = Mathf::Spherical2Cartesian (theta, phi, 1.0f);
				vertices.push_back (cartesian.GetX ());
				vertices.push_back (cartesian.GetY ());
				vertices.push_back (cartesian.GetZ ());
			}
		}

		vertices.push_back (0.0f);
		vertices.push_back (1.0f);
		vertices.push_back (0.0f);

		vertices.push_back (0.0f);
		vertices.push_back (-1.0f);
		vertices.push_back (0.0f);

		// generate face data
		for (unsigned int i = 0; i < latSteps - 2; i++)
		{
			for (unsigned int j = 0; j <= lonSteps - 1; j++)
			{
				faces.push_back (i * lonSteps + j);
				faces.push_back (i * lonSteps + (j + 1) % lonSteps);
				faces.push_back ((i + 1) * lonSteps + j);

				faces.push_back (i * lonSteps + (j + 1) % lonSteps);
				faces.push_back ((i + 1) * lonSteps + (j + 1) % lonSteps);
				faces.push_back ((i + 1) * lonSteps + j);
			}
		}

		nIndex = (latSteps - 1) * lonSteps;
		sIndex = nIndex + 1;

		for (unsigned int j = 0; j <= lonSteps - 1; j++)
		{
			faces.push_back (nIndex);
			faces.push_back ((j + 1) % lonSteps);
			faces.push_back (j);

			faces.push_back (sIndex);
			faces.push_back (nIndex - 1 - (j + 1) % lonSteps);
			faces.push_back (nIndex - 1 - j);
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
			if ((faces[f] == v / 3) || (faces[f + 1] == v / 3) || (faces[f + 2] == v / 3))
			{
				v1 = Vector3f (vertices[faces[f] * 3], vertices[faces[f] * 3 + 1], vertices[faces[f] * 3 + 2]);
				v2 = Vector3f (vertices[faces[f + 1] * 3], vertices[faces[f + 1] * 3 + 1], vertices[faces[f + 1] * 3 + 2]);
				v3 = Vector3f (vertices[faces[f + 2] * 3], vertices[faces[f + 2] * 3 + 1], vertices[faces[f + 2] * 3 + 2]);

				normal += Cross (v2 - v1, v3 - v1);

			}
		}

		// normalize
		normal.Normalize ();

		// store normal
		normals.push_back (normal.GetX ());
		normals.push_back (normal.GetY ());
		normals.push_back (normal.GetZ ());
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



