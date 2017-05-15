#ifndef _MESHOBJECT_H_
#define _MESGOBJECT_H_

#include <vector>
#include "Vector4f.h"
#include "GameObject.h"

// Represents a 3d mesh, inherits from GameObject
// 3d mesh has a collection of vertices and a collection of vertex indices to represent mesh faces (polygons)
// 3d mesh may have only one material assigned to it
class MeshObject : public GameObject
{
	std::vector<Vector4f> vertices;
	std::vector<int> faces;
public:
	MeshObject ();
	~MeshObject ();
	MeshObject& Clear ();
	MeshObject& InitUnitCube ();
	MeshObject& InitUnitPlane ();
};

#endif