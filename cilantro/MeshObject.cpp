#include "MeshObject.h"
#include "Renderer.h"
#include "LogMessage.h"

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
	
	normals = std::vector<float> {

		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,

		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f

	};

	// CalculateVertexNormals ();

	InvokeCallbacks ("OnUpdateMeshObject", this->GetHandle ());

	return *this;
}

void MeshObject::CalculateVertexNormals ()
{
	// TBD
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



