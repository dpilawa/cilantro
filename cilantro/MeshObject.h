#ifndef _MESHOBJECT_H_
#define _MESHOBJECT_H_

#include <vector>
#include "Vector4f.h"
#include "Material.h"
#include "GameObject.h"

// Represents a 3d mesh, inherits from GameObject
// 3d mesh has a collection of vertices and a collection of vertex indices to represent mesh faces (polygons)
// 3d mesh may have only one material assigned to it
class MeshObject : public GameObject
{
public:
	MeshObject ();
	~MeshObject ();

	// Cleans up contents of used collections
	MeshObject& Clear ();

	// Initialize mesh in local space as unit cube with center in space origin
	MeshObject& InitUnitCube ();

	// calculate vertex normals
	void CalculateVertexNormals ();

	// get mesh counts
	unsigned int GetVertexCount ();
	unsigned int GetFaceCount ();

	// set mesh material
	void SetMaterial (Material& material);

	// get mesh material
	Material& GetMaterial () const;

	// get vertices raw data
	float* GetVerticesData ();

	// get normals raw data
	float* GetNormalsData ();

	// get faces raw data
	unsigned int* GetFacesData ();

	// invoked by game loop on each frame
	virtual void OnFrame ();
	virtual void OnDraw (Renderer& renderer);

private:

	Material* objectMaterial;

	std::vector<float> vertices;
	std::vector<unsigned int> faces;

	std::vector<float> normals;
	std::vector<float> uvs;
};

#endif