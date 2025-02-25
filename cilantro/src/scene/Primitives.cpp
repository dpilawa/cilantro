#include "scene/Primitives.h"
#include "resource/Mesh.h"
#include "math/Vector2f.h"
#include "math/Vector3f.h"
#include "math/Mathf.h"

void Primitives::GeneratePlane (Mesh& m)
{
    m.Clear ();

    std::vector<Vector3f> vertices {
        { 1.0f, 0.0f,  1.0f}, 
        {-1.0f, 0.0f,  1.0f}, 
        {-1.0f, 0.0f, -1.0f}, 
        { 1.0f, 0.0f, -1.0f}
    };

    std::vector<std::size_t> varray {
        1, 3, 2,
        1, 0, 3
    };

    std::vector<Vector2f> uv {
        {0.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f},
        {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}
    };

    Primitives::GenerateMeshFromArrays (m, vertices, uv, varray);
    
    m.CalculateVertexNormals ();
    m.CalculateTangentsBitangents ();

    m.InvokeHook ("OnUpdateMesh");
    
}

void Primitives::GenerateCube (Mesh& m)
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

    std::vector<std::size_t> varray {
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

    std::vector<Vector2f> uv {
        {0.500f, 1.000f}, {0.250f, 1.000f}, {0.250f, 0.666f},
        {0.500f, 1.000f}, {0.250f, 0.666f}, {0.500f, 0.666f},

        {0.250f, 0.333f}, {0.250f, 0.000f}, {0.500f, 0.000f},
        {0.500f, 0.333f}, {0.250f, 0.333f}, {0.500f, 0.000f},

        {1.000f, 0.666f}, {0.750f, 0.333f},  {1.000f, 0.333f},
        {0.750f, 0.666f}, {0.750f, 0.333f}, {1.000f, 0.666f},

        {0.500f, 0.333f}, {0.500f, 0.666f}, {0.250f, 0.666f},
        {0.250f, 0.666f}, {0.250f, 0.333f}, {0.500f, 0.333f},

        {0.750f, 0.666f}, {0.500f, 0.333f}, {0.750f, 0.333f},
        {0.750f, 0.666f}, {0.500f, 0.666f}, {0.500f, 0.333f},

        {0.250f, 0.333f}, {0.250f, 0.666f}, {0.000f, 0.666f},
        {0.000f, 0.666f}, {0.000f, 0.333f}, {0.250f, 0.333f}
    };

    Primitives::GenerateMeshFromArrays (m, vertices, uv, varray);

    m.CalculateVertexNormals ();
    m.CalculateTangentsBitangents ();

    m.InvokeHook ("OnUpdateMesh");
    
}

void Primitives::GenerateSphere (Mesh& m, unsigned int subdivisions)
{
    float step;
    float theta, phi;
    unsigned int lonSteps, latSteps;
    unsigned int nIndex, sIndex;

    std::vector<Vector3f> vertices;
    std::vector<Vector2f> uv;
    std::vector<std::size_t> varray;

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

            Vector3f vertex = Mathf::Spherical2Cartesian (theta, phi, 1.0f);
            vertices.push_back (vertex);
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
            varray.push_back (i * lonSteps + j);
            varray.push_back (i * lonSteps + (j + 1) % lonSteps);
            varray.push_back ((i + 1) * lonSteps + j);

            uv.push_back (Vector2f (1.0f - (float)j / float(lonSteps), 1.0f - (float)i / float(latSteps)));
            uv.push_back (Vector2f (1.0f - (float)(j + 1) / float(lonSteps), 1.0f - (float)i / float(latSteps)));
            uv.push_back (Vector2f (1.0f - (float)j / float(lonSteps), 1.0f - (float)(i + 1) / float(latSteps)));

            varray.push_back (i * lonSteps + (j + 1) % lonSteps);
            varray.push_back ((i + 1) * lonSteps + (j + 1) % lonSteps);
            varray.push_back ((i + 1) * lonSteps + j);

            uv.push_back (Vector2f (1.0f - (float)(j + 1) / float(lonSteps), 1.0f - (float)i / float(latSteps)));
            uv.push_back (Vector2f (1.0f - (float)(j + 1) / float(lonSteps), 1.0f - (float)(i + 1) / float(latSteps)));
            uv.push_back (Vector2f (1.0f - (float)j / float(lonSteps), 1.0f - (float)(i + 1) / float(latSteps)));
        }
    }

    // pole faces
    nIndex = (latSteps - 1) * lonSteps;
    sIndex = nIndex + 1;

    for (unsigned int j = 0; j <= lonSteps - 1; j++)
    {
        varray.push_back (nIndex);
        varray.push_back ((j + 1) % lonSteps);
        varray.push_back (j);

        uv.push_back (Vector2f (1.0f - (float)j / float(lonSteps), 1.0f - (float)0.0f / float(latSteps)));
        uv.push_back (Vector2f (1.0f - (float)(j + 1) / float(lonSteps), 1.0f - (float)1.0f / float(latSteps)));
        uv.push_back (Vector2f (1.0f - (float)j / float(lonSteps), 1.0f - (float)1.0f / float(latSteps)));

        varray.push_back (sIndex);
        varray.push_back (nIndex - 1 - (j + 1) % lonSteps);
        varray.push_back (nIndex - 1 - j);

        uv.push_back (Vector2f (1.0f - (float)j / float(lonSteps), (float)0.0f / float(latSteps)));
        uv.push_back (Vector2f (1.0f - (float)(j + 1) / float(lonSteps), (float)1.0f / float(latSteps)));
        uv.push_back (Vector2f (1.0f - (float)j / float(lonSteps), (float)1.0f / float(latSteps)));
    }

    Primitives::GenerateMeshFromArrays (m, vertices, uv, varray);

    m.CalculateVertexNormals ();
    m.CalculateTangentsBitangents ();

    m.InvokeHook ("OnUpdateMesh");

}

void Primitives::GenerateCone (Mesh& m, unsigned int subdivisions)
{
    float step;
    float phi;
    unsigned int lonSteps;

    std::vector<Vector3f> vertices;
    std::vector<Vector2f> uv;
    std::vector<std::size_t> varray;

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
        varray.push_back ((i + 1) % lonSteps);
        varray.push_back (i);
        varray.push_back (lonSteps);

        uv.push_back (Mathf::Spherical2Cartesian ((float)(i + 1) * (2.0f * Mathf::Pi () / float(lonSteps)), 0.5f) + Vector2f (0.5f, 0.5f));
        uv.push_back (Mathf::Spherical2Cartesian ((float)i * (2.0f * Mathf::Pi () / float(lonSteps)), 0.5f) + Vector2f (0.5f, 0.5f));
        uv.push_back (Vector2f (0.5f, 0.5f));

        varray.push_back (i);
        varray.push_back ((i + 1) % lonSteps);
        varray.push_back (lonSteps + 1);

        uv.push_back (Mathf::Spherical2Cartesian ((float)i * (2.0f * Mathf::Pi () / float(lonSteps)), 0.5f) + Vector2f (0.5f, 0.5f));
        uv.push_back (Mathf::Spherical2Cartesian ((float)(i + 1) * (2.0f * Mathf::Pi () / float(lonSteps)), 0.5f) + Vector2f (0.5f, 0.5f));
        uv.push_back (Vector2f (0.5f, 0.5f));
    }

    Primitives::GenerateMeshFromArrays (m, vertices, uv, varray);

    m.CalculateVertexNormals ();
    m.CalculateTangentsBitangents ();

    m.InvokeHook ("OnUpdateMesh");

}

void Primitives::GenerateCylinder (Mesh& m, unsigned int subdivisions)
{
    float step;
    float phi;
    unsigned int lonSteps;
    unsigned int nIndex, sIndex;

    std::vector<Vector3f> vertices;
    std::vector<Vector2f> uv;
    std::vector<std::size_t> varray;

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
        varray.push_back (i);
        varray.push_back ((i + 2) % (lonSteps * 2));
        varray.push_back ((i + 1) % (lonSteps * 2));

        uv.push_back (Vector2f ((float)(i / 2) / float(lonSteps), 1.0f));
        uv.push_back (Vector2f ((float)((i / 2) + 1) / float(lonSteps), 1.0f));        
        uv.push_back (Vector2f ((float)(i / 2) / float(lonSteps), 0.0f));

        varray.push_back ((i + 1) % (lonSteps * 2));
        varray.push_back ((i + 2) % (lonSteps * 2));
        varray.push_back ((i + 3) % (lonSteps * 2));

        uv.push_back (Vector2f ((float)(i / 2) / float(lonSteps), 0.0f));
        uv.push_back (Vector2f ((float)((i / 2) + 1) / float(lonSteps), 1.0f));
        uv.push_back (Vector2f ((float)((i / 2) + 1) / float(lonSteps), 0.0f));

        varray.push_back ((i + 2) % (lonSteps * 2));
        varray.push_back (i);
        varray.push_back (nIndex);

        uv.push_back (Mathf::Spherical2Cartesian ((float)((i / 2) + 1) * (2.0f * Mathf::Pi () / float(lonSteps)), 0.5f) + Vector2f (0.5f, 0.5f));
        uv.push_back (Mathf::Spherical2Cartesian ((float)(i / 2) * (2.0f * Mathf::Pi () / float(lonSteps)), 0.5f) + Vector2f (0.5f, 0.5f));
        uv.push_back (Vector2f (0.5f, 0.5f));

        varray.push_back ((i + 1) % (lonSteps * 2));
        varray.push_back ((i + 3) % (lonSteps * 2));
        varray.push_back (sIndex);    

        uv.push_back (Mathf::Spherical2Cartesian ((float)(i / 2) * (2.0f * Mathf::Pi () / float(lonSteps)), 0.5f) + Vector2f (0.5f, 0.5f));
        uv.push_back (Mathf::Spherical2Cartesian ((float)((i / 2) + 1) * (2.0f * Mathf::Pi () / float(lonSteps)), 0.5f) + Vector2f (0.5f, 0.5f));
        uv.push_back (Vector2f (0.5f, 0.5f));        
    }

    Primitives::GenerateMeshFromArrays (m, vertices, uv, varray);

    m.CalculateVertexNormals ();
    m.CalculateTangentsBitangents ();

    m.InvokeHook ("OnUpdateMesh");

}

void Primitives::GenerateMeshFromArrays (Mesh& m, const std::vector<Vector3f>& vertices, const std::vector<Vector2f>& uv, const std::vector<std::size_t> varray)
{

    for (std::size_t i = 0; i < varray.size (); i += 3)
    {
        m.AddVertex (vertices[varray[i]], uv[i]);
        m.AddVertex (vertices[varray[i + 1]], uv[i + 1]);
        m.AddVertex (vertices[varray[i + 2]], uv[i + 2]);
    }

    for (std::size_t i = 0; i < varray.size (); i += 3)
    {
        m.AddFace (i, i + 1, i + 2);
    }

}