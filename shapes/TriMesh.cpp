/*
 * TriMesh.cpp
 *
 *
 */
#include "TriMesh.h"
#include <algorithm> 

namespace rt{

    /**
     * Factory function that returns trimesh subclass based on trimesh specifications
     * @param nfaces the number of faces in a trimesh
     * @param faceIndex the indices of faces
     * @param vertsIndex the indices of vertices
     * @param verts the vertices of triangles 
     * @param normals the triangle normals
     * @param st the texture mapping coordinates
     * @param material the material of trimesh object
     *
     * @return trimesh subclass instance
     *
     */
    TriMesh::TriMesh(
        const uint32_t nfaces,
        const std::unique_ptr<uint32_t[]>& faceIndex,
        const std::unique_ptr<uint32_t[]>& vertsIndex,
        const std::unique_ptr<Vec3f[]>& verts,
        std::unique_ptr<Vec3f[]>& normals,
        std::unique_ptr<Vec2f[]>& st,
        Material* material) :
        numTris(0), material(material), Shape(material)
    {
        uint32_t k = 0, maxVertIndex = 0;
        // find out how many triangles we need to create for this mesh
        for (uint32_t i = 0; i < nfaces; ++i) {
            numTris += faceIndex[i] - 2;
            for (uint32_t j = 0; j < faceIndex[i]; ++j)
                if (vertsIndex[k + j] > maxVertIndex)
                    maxVertIndex = vertsIndex[k + j];
            k += faceIndex[i];
        }
        maxVertIndex += 1;

        // allocate memory to store the position of the mesh vertices
        P = std::unique_ptr<Vec3f[]>(new Vec3f[maxVertIndex]);
        for (uint32_t i = 0; i < maxVertIndex; ++i) {
            P[i] = verts[i];
        }

        // allocate memory to store triangle indices
        trisIndex = std::unique_ptr<uint32_t[]>(new uint32_t[numTris * 3]);
        uint32_t l = 0;

        // Generate the triangle index array
        N = std::unique_ptr<Vec3f[]>(new Vec3f[numTris * 3]);
        texCoordinates = std::unique_ptr<Vec2f[]>(new Vec2f[numTris * 3]);
        for (uint32_t i = 0, k = 0; i < nfaces; ++i) { // for each  face
            for (uint32_t j = 0; j < faceIndex[i] - 2; ++j) { // for each triangle in the face
                trisIndex[l] = vertsIndex[k];
                trisIndex[l + 1] = vertsIndex[k + j + 1];
                trisIndex[l + 2] = vertsIndex[k + j + 2];
                N[l] = normals[k];
                N[l + 1] = normals[k + j + 1];
                N[l + 2] = normals[k + j + 2];
                texCoordinates[l] = st[k];
                texCoordinates[l + 1] = st[k + j + 1];
                texCoordinates[l + 2] = st[k + j + 2];
                l += 3;
            }
            k += faceIndex[i];
        }
    }


   
    


} //namespace rt


