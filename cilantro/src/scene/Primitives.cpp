#include "scene/Primitives.h"
#include "math/Mathf.h"

void Primitives::GeneratePlane (MeshObject& m, bool sharedVertices)
{
    m.Clear ();

    std::vector<Vector3f> vertices {
        { 1.0f, 0.0f,  1.0f}, 
        {-1.0f, 0.0f,  1.0f}, 
        {-1.0f, 0.0f, -1.0f}, 
        { 1.0f, 0.0f, -1.0f}
    };

    std::vector<std::size_t> array {
        1, 3, 2,
        1, 0, 3
    };

    Primitives::GenerateMeshFromArrays (m, sharedVertices, vertices, array);
    
    m.CalculateVertexNormals ();

    m.InvokeCallbacks ("OnUpdateMeshObject", m.GetHandle ());
    
}

void Primitives::GenerateCube (MeshObject& m, bool sharedVertices)
{
    m.Clear ();

    std::vector<Vector3f> vertices {
        { 1.0f, 1.0f, -1.0f},
        {-1.0f, 1.0f, -1.0f},
        {-1.0f, 1.0f,  1.0f},
        { 1.0f, 1.0f,  1.0f},

        { 1.0f, -1.0f, -1.0f},
        {-1.0f, -1.0f, -1.0f},
        {-1.0f, -1.0f,  1.0f},
        { 1.0f, -1.0f,  1.0f}
    };

    std::vector<std::size_t> array {
        0, 1, 2,
        0, 2, 3,

        6, 5, 4,
        7, 6, 4,

        1, 4, 5,
        0, 4, 1,

        7, 3, 2,
        2, 6, 7,

        0, 7, 4,
        0, 3, 7,

        6, 2, 1,
        1, 5, 6
    };

    Primitives::GenerateMeshFromArrays (m, sharedVertices, vertices, array);

    m.CalculateVertexNormals ();

    m.InvokeCallbacks ("OnUpdateMeshObject", m.GetHandle ());
    
}

void Primitives::GenerateSphere (MeshObject& m, unsigned int subdivisions, bool sharedVertices)
{
    float step;
    float theta, phi;
    unsigned int lonSteps, latSteps;
    unsigned int nIndex, sIndex;

    std::vector<Vector3f> vertices;
    std::vector<std::size_t> array;

    m.Clear ();

    step = Mathf::Pi () / ((subdivisions + 1) * 2);
    lonSteps = (subdivisions + 1) * 4;
    latSteps = (subdivisions + 1) * 2;

    // generate sphere vertices
    for (unsigned int i = 1; i <= latSteps - 1; i++)
    {
        theta = step * i;

        for (unsigned int j = 0; j <= lonSteps - 1; j++)
        {
            phi = step * j;
            vertices.push_back (Mathf::Spherical2Cartesian (theta, phi, 1.0f));
        }
    }

    // pole vertices
    vertices.push_back (Vector3f (0.0f, 1.0f, 0.0f));
    vertices.push_back (Vector3f (0.0f, -1.0f, 0.0f));

    // generate face array
    for (unsigned int i = 0; i < latSteps - 2; i++)
    {
        for (unsigned int j = 0; j <= lonSteps - 1; j++)
        {
            array.push_back (i * lonSteps + j);
            array.push_back (i * lonSteps + (j + 1) % lonSteps);
            array.push_back ((i + 1) * lonSteps + j);

            array.push_back (i * lonSteps + (j + 1) % lonSteps);
            array.push_back ((i + 1) * lonSteps + (j + 1) % lonSteps);
            array.push_back ((i + 1) * lonSteps + j);
        }
    }

    // pole faces
    nIndex = (latSteps - 1) * lonSteps;
    sIndex = nIndex + 1;

    for (unsigned int j = 0; j <= lonSteps - 1; j++)
    {
        array.push_back (nIndex);
        array.push_back ((j + 1) % lonSteps);
        array.push_back (j);

        array.push_back (sIndex);
        array.push_back (nIndex - 1 - (j + 1) % lonSteps);
        array.push_back (nIndex - 1 - j);
    }

    Primitives::GenerateMeshFromArrays (m, sharedVertices, vertices, array);

    m.CalculateVertexNormals ();

    m.InvokeCallbacks ("OnUpdateMeshObject", m.GetHandle ());

}

void Primitives::GenerateCone (MeshObject& m, unsigned int subdivisions, bool sharedVertices)
{
    float step;
    float phi;
    unsigned int lonSteps;

    std::vector<Vector3f> vertices;
    std::vector<std::size_t> array;

    m.Clear ();

    step = Mathf::Pi () / ((subdivisions + 1) * 2);
    lonSteps = (subdivisions + 1) * 4;

    // generate base vertices
    for (unsigned int j = 0; j <= lonSteps - 1; j++)
    {
        phi = step * j;
        vertices.push_back (Mathf::Spherical2Cartesian (Mathf::Pi () / 2.0f, phi, 1.0f) + Vector3f (0.0f, -1.0f, 0.0f));
    }

    // apex and base center
    vertices.push_back (Vector3f (0.0f, 1.0f, 0.0f));
    vertices.push_back (Vector3f (0.0f, -1.0f, 0.0f));

    // generate face array
    for (unsigned int i = 0; i <= lonSteps - 1; i++)
    {
        array.push_back ((i + 1) % lonSteps);
        array.push_back (i);
        array.push_back (lonSteps);

        array.push_back (i);
        array.push_back ((i + 1) % lonSteps);
        array.push_back (lonSteps + 1);
    }

    Primitives::GenerateMeshFromArrays (m, sharedVertices, vertices, array);

    m.CalculateVertexNormals ();

    m.InvokeCallbacks ("OnUpdateMeshObject", m.GetHandle ());

}

void Primitives::GenerateCylinder (MeshObject& m, unsigned int subdivisions, bool sharedVertices)
{
    float step;
    float phi;
    unsigned int lonSteps;
    unsigned int nIndex, sIndex;

    std::vector<Vector3f> vertices;
    std::vector<std::size_t> array;

    m.Clear ();

    step = Mathf::Pi () / ((subdivisions + 1) * 2);
    lonSteps = (subdivisions + 1) * 4;

    // generate base vertices
    for (unsigned int j = 0; j <= lonSteps - 1; j++)
    {
        phi = step * j;
        vertices.push_back (Mathf::Spherical2Cartesian (Mathf::Pi () / 2.0f, phi, 1.0f) + Vector3f (0.0f, 1.0f, 0.0f));
        vertices.push_back (Mathf::Spherical2Cartesian (Mathf::Pi () / 2.0f, phi, 1.0f) + Vector3f (0.0f, -1.0f, 0.0f));
    }

    // base centers
    vertices.push_back (Vector3f (0.0f, 1.0f, 0.0f));
    vertices.push_back (Vector3f (0.0f, -1.0f, 0.0f));
    nIndex = lonSteps * 2;
    sIndex = nIndex + 1;

    // generate face array
    for (unsigned int i = 0; i <= 2 * (lonSteps - 1); i += 2)
    {
        array.push_back (i);
        array.push_back ((i + 2) % (lonSteps * 2));
        array.push_back ((i + 1) % (lonSteps * 2));

        array.push_back (i);
        array.push_back (nIndex);
        array.push_back ((i + 2) % (lonSteps * 2));

        array.push_back ((i + 1) % (lonSteps * 2));
        array.push_back ((i + 2) % (lonSteps * 2));
        array.push_back ((i + 3) % (lonSteps * 2));

        array.push_back ((i + 1) % (lonSteps * 2));
        array.push_back ((i + 3) % (lonSteps * 2));
        array.push_back (sIndex);    
    }

    Primitives::GenerateMeshFromArrays (m, sharedVertices, vertices, array);

    m.CalculateVertexNormals ();

    m.InvokeCallbacks ("OnUpdateMeshObject", m.GetHandle ());

}

void Primitives::GenerateMeshFromArrays (MeshObject& m, bool sharedVertices, const std::vector<Vector3f>& vertices, const std::vector<std::size_t> array)
{
    if (sharedVertices == false)
    {
        for (std::size_t i = 0; i < array.size (); i += 3)
        {
            m.AddVertex (vertices[array[i]]);
            m.AddVertex (vertices[array[i + 1]]);
            m.AddVertex (vertices[array[i + 2]]);
        }

        for (std::size_t i = 0; i < array.size (); i += 3)
        {
            m.AddFace (i, i + 1, i + 2);
        }
    }
    else
    {        
        for (std::size_t i = 0; i < vertices.size (); i++)
        {
            m.AddVertex (vertices[i]);
        }

        for (std::size_t i = 0; i < array.size (); i += 3)
        {
            m.AddFace (array[i], array[i + 1], array[i + 2]);
        }
    }
}