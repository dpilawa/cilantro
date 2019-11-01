#ifndef _MESHOBJECT_H_
#define _MESHOBJECT_H_

#include "cilantroengine.h"
#include "GameObject.h"
#include "Material.h"
#include "Vector3f.h"
#include "Spline.h"
#include <vector>

class Renderer;

enum class MeshType
{
	Points,
	Lines,
	Triangles
};

// Represents a 3d mesh, inherits from GameObject
// 3d mesh has a collection of vertices and a collection of vertex indices to represent mesh faces (polygons)
// 3d mesh may have only one material assigned to it
class MeshObject : public GameObject
{
public:
	__EAPI MeshObject (MeshType type = MeshType::Triangles);
	__EAPI virtual ~MeshObject ();

	// Cleans up contents of used collections
	__EAPI MeshObject& Clear ();

	// Initialize mesh in local space as unit primitives with center in space origin
	__EAPI MeshObject& InitUnitCube ();
	__EAPI MeshObject& InitUnitSphere (unsigned int subdivisions, bool sharedVertices = false);
    __EAPI MeshObject& InitSpline (const Spline<Vector3f>& spline, unsigned int segments, bool showControlLines = false);

    // calculate vertex normals
	__EAPI void CalculateVertexNormals ();

	// get mesh counts
	__EAPI unsigned int GetVertexCount ();
	__EAPI unsigned int GetIndexCount ();

    // set mesh material
	__EAPI void SetMaterial (Material& material);

	// get mesh material
	__EAPI Material& GetMaterial () const;

	// get vertices raw data
	__EAPI float* GetVerticesData ();

	// get normals raw data
	__EAPI float* GetNormalsData ();

	// get faces raw data
	__EAPI unsigned int* GetFacesData ();

	// get mesh type
    __EAPI MeshType GetMeshType ();

    // add vertices and primitives
	__EAPI unsigned int AddVertex (const Vector3f& vertex);
	__EAPI void AddPoint (unsigned int v, bool rendererUpdate = true);
	__EAPI void AddLine (unsigned int v1, unsigned int v2, bool rendererUpdate = true);
	__EAPI void AddFace (unsigned int v1, unsigned int v2, unsigned int v3, bool rendererUpdate = true);

	// invoked by game loop on each frame or on update (e.g. transform)
	virtual void OnFrame ();
	virtual void OnDraw (Renderer& renderer);
	virtual void OnUpdate (Renderer& renderer);

private:

    MeshType meshType;
    Material* objectMaterial;

    std::vector<float> vertices;
	std::vector<unsigned int> indices;

	std::vector<float> normals;
	std::vector<float> uvs;
};

#endif
