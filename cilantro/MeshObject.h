#ifndef _MESHOBJECT_H_
#define _MESHOBJECT_H_

#include <vector>
#include "Vector4f.h"
#include "GameObject.h"

// Represents a 3d mesh, inherits from GameObject
// 3d mesh has a collection of vertices and a collection of vertex indices to represent mesh faces (polygons)
// 3d mesh may have only one material assigned to it
class MeshObject : public GameObject
{
	std::vector<float> vertices;
	std::vector<unsigned int> faces;

public:
	MeshObject ();
	~MeshObject ();

	// Cleans up contents of used collections
	MeshObject& Clear ();

	// Initialize mesh in local space as unit cube with center in space origin
	MeshObject& InitUnitCube ();

	// Initialize mesh in local space as unit plane parallel to x and z axis, with center in space origin
	MeshObject& InitUnitPlane ();

	// get mesh counts
	unsigned int GetVertexCount ();
	unsigned int GetFaceCount ();

	// get vertices raw data
	float* GetVerticesData ();

	// get faces raw data
	unsigned int* GetFacesData ();

	// invoked by game loop on each frame
	virtual void OnFrame ();
	virtual void OnDraw (Renderer& renderer);

};

#endif