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

    m.Clear ();

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

                m.AddVertex (Mathf::Spherical2Cartesian (theta, phi, 1.0f));
                m.AddVertex (Mathf::Spherical2Cartesian (theta, phi + step, 1.0f));
                m.AddVertex (Mathf::Spherical2Cartesian (theta + step, phi, 1.0f));

                m.AddVertex (Mathf::Spherical2Cartesian (theta, phi + step, 1.0f));
                m.AddVertex (Mathf::Spherical2Cartesian (theta + step, phi + step, 1.0f));
                m.AddVertex (Mathf::Spherical2Cartesian (theta + step, phi, 1.0f));
            }
        }

        theta = 0.0f;
        for (unsigned int j = 0; j <= lonSteps - 1; j++)
        {
            phi = step * j;

            m.AddVertex (Mathf::Spherical2Cartesian (theta + step, phi, 1.0f));
            m.AddVertex (Mathf::Spherical2Cartesian (theta, phi, 1.0f));
            m.AddVertex (Mathf::Spherical2Cartesian (theta + step, phi + step, 1.0f));
        }

        theta = Mathf::Pi ();
        for (unsigned int j = 0; j <= lonSteps - 1; j++)
        {
            phi = step * j;

            m.AddVertex (Mathf::Spherical2Cartesian (theta, phi, 1.0f));
            m.AddVertex (Mathf::Spherical2Cartesian (theta - step, phi, 1.0f));
            m.AddVertex (Mathf::Spherical2Cartesian (theta - step, phi + step, 1.0f));
        }

        // generate faces
        for (unsigned int i = 0; i < latSteps * lonSteps * 3 * 2; i += 3)
        {
            m.AddFace (i, i + 1, i + 2);
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
                m.AddVertex (Mathf::Spherical2Cartesian (theta, phi, 1.0f));
            }
        }

        // add poles
        m.AddVertex (Vector3f (0.0f, 1.0f, 0.0f));
        m.AddVertex (Vector3f (0.0f, -1.0f, 0.0f));

        // generate face data
        for (unsigned int i = 0; i < latSteps - 2; i++)
        {
            for (unsigned int j = 0; j <= lonSteps - 1; j++)
            {
                m.AddFace (i * lonSteps + j, i * lonSteps + (j + 1) % lonSteps, (i + 1) * lonSteps + j);
                m.AddFace (i * lonSteps + (j + 1) % lonSteps, (i + 1) * lonSteps + (j + 1) % lonSteps, (i + 1) * lonSteps + j);
            }
        }

        nIndex = (latSteps - 1) * lonSteps;
        sIndex = nIndex + 1;

        for (unsigned int j = 0; j <= lonSteps - 1; j++)
        {
            m.AddFace (nIndex, (j + 1) % lonSteps, j);
            m.AddFace (sIndex, nIndex - 1 - (j + 1) % lonSteps, nIndex - 1 - j);
        }

    }

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