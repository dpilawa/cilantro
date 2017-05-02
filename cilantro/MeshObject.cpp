#include <vector>
#include "Vector4f.h"
#include "MeshObject.h"

MeshObject::MeshObject ()
{
}

MeshObject::~MeshObject ()
{
}

// Cleans up contents of used collections
void MeshObject::Clear ()
{
	vertices.clear ();
	faces.clear ();
}

// Initialize mesh in local space as unit cube with center in space origin
void MeshObject::InitUnitCube ()
{
	Clear ();
	// fixme - winding
	vertices.push_back (Vector4f (.5f, .5f, .5f));
	vertices.push_back (Vector4f (-.5f, .5f, .5f));
	vertices.push_back (Vector4f (.5f, -.5f, .5f));
	vertices.push_back (Vector4f (-.5f, -.5f, .5f));

	vertices.push_back (Vector4f (.5f, .5f, -.5f));
	vertices.push_back (Vector4f (-.5f, .5f, -.5f));
	vertices.push_back (Vector4f (.5f, -.5f, -.5f));
	vertices.push_back (Vector4f (-.5f, -.5f, -.5f));

	faces = std::vector<int> { 0, 1, 2, 3, 5, 6 };
}

// Initialize mesh in local space as unit plane parallel to x and z axis, with center in space origin
void MeshObject::InitUnitPlane ()
{
	Clear ();

	vertices.push_back (Vector4f (.5f, 0, .5f));
	vertices.push_back (Vector4f (.5f, 0, -.5f));
	vertices.push_back (Vector4f (-.5f, 0, -.5f));
	vertices.push_back (Vector4f (-.5f, 0, .5f));

	faces = std::vector<int> { 0, 1, 2, 2, 3, 0 };
}