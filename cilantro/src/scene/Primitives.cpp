#include "scene/Primitives.h"
#include "math/Mathf.h"

void Primitives::GenerateCube (MeshObject& m)
{
    m.Clear ();

    std::vector<float> vertices {

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

    for (unsigned int i = 0; i < vertices.size (); i += 3)
    {
        m.AddVertex (Vector3f (vertices[i], vertices[i + 1], vertices[i + 2]));
    }

    for (unsigned int i = 0; i < 36; i += 3)
    {
        m.AddFace (i, i + 1, i + 2);
    };
    
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