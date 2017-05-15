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
MeshObject& MeshObject::Clear ()
{
	vertices.clear ();
	faces.clear ();

	return *this;
}

// Initialize mesh in local space as unit cube with center in space origin
MeshObject& MeshObject::InitUnitCube ()
{
	Clear ();

	vertices.push_back (Vector4f (.5f, .5f, .5f));
	vertices.push_back (Vector4f (-.5f, .5f, .5f));
	vertices.push_back (Vector4f (-.5f, -.5f, .5f));
	vertices.push_back (Vector4f (.5f, -.5f, .5f));

	vertices.push_back (Vector4f (.5f, .5f, -.5f));
	vertices.push_back (Vector4f (-.5f, .5f, -.5f));
	vertices.push_back (Vector4f (-.5f, -.5f, -.5f));
	vertices.push_back (Vector4f (.5f, -.5f, -.5f));

	faces = std::vector<int> { 0, 1, 2, 0, 2, 3, 6, 5, 4, 4, 7, 6,		// front, back
							   0, 4, 1, 1, 4, 5, 3, 2, 6, 3, 6, 7,		// top, bottom
							   1, 5, 6, 1, 6, 2, 0, 3, 4, 3, 7, 4 };	// left, right

	return *this;
}

// Initialize mesh in local space as unit plane parallel to x and z axis, with center in space origin
MeshObject& MeshObject::InitUnitPlane ()
{
	Clear ();

	vertices.push_back (Vector4f (.5f, 0, .5f));
	vertices.push_back (Vector4f (.5f, 0, -.5f));
	vertices.push_back (Vector4f (-.5f, 0, -.5f));
	vertices.push_back (Vector4f (-.5f, 0, .5f));

	faces = std::vector<int> { 0, 1, 2, 2, 3, 0 };

	return *this;
}