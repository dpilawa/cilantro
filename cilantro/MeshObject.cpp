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
	faces.clear ();

	return *this;
}


MeshObject& MeshObject::InitUnitCube ()
{
	Clear ();

	vertices = std::vector<float> { .5f, .5f, .5f,
									-.5f, .5f, .5f,
									-.5f, -.5f, .5f,
									.5f, -.5f, .5f,
									.5f, .5f, -.5f,
									-.5f, .5f, -.5f,
									-.5f, -.5f, -.5f,
									.5f, -.5f, -.5f };

	faces = std::vector<unsigned int> { 0, 1, 2, 0, 2, 3, 6, 5, 4, 4, 7, 6,		// front, back
										0, 4, 1, 1, 4, 5, 3, 2, 6, 3, 6, 7,		// top, bottom
										1, 5, 6, 1, 6, 2, 0, 3, 4, 3, 7, 4 };	// left, right

	InvokeCallbacks ("OnUpdateMeshObject", this->GetHandle ());

	return *this;
}


MeshObject& MeshObject::InitUnitPlane ()
{
	Clear ();

	vertices = std::vector<float> { .5f, 0, .5f,
									.5f, 0, -.5f,
									-.5f, 0, -.5f,
									-.5f, 0, .5f };

	faces = std::vector<unsigned int> { 0, 1, 2, 2, 3, 0 };

	InvokeCallbacks ("OnUpdateMeshObject", this->GetHandle ());

	return *this;
}

unsigned int MeshObject::GetVertexCount ()
{
	return (unsigned int) vertices.size ();
}

unsigned int MeshObject::GetFaceCount ()
{
	return (unsigned int) faces.size ();
}

float* MeshObject::GetVerticesData ()
{
	return vertices.data ();
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



