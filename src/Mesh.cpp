#include <simple-animation-blender/Mesh.h>
using namespace Assimp;
using namespace glm;
Mesh::Mesh(const char *path)
{
    Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate);
    aiMesh *mesh = scene->mMeshes[0];
    vertices.resize(mesh->mNumVertices);
    for (uint32_t i = 0; i < vertices.size(); ++i)
    {
        vertices[i].pos = vec3(
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z);
        vertices[i].normal = vec3(
            mesh->mNormals[i].x,
            mesh->mNormals[i].y,
            mesh->mNormals[i].z);
    }
    for (uint32_t i = 0; i < mesh->mNumBones; ++i)
    {
        for (uint32_t j = 0; j < mesh->mBones[i]->mNumWeights; ++j)
        {
            int vertexID = mesh->mBones[i]->mWeights[j].mVertexId;
            float weight = mesh->mBones[i]->mWeights[j].mWeight;
            if (weight != 0.0f)
            {
                if (vertices[vertexID].bonesIndices.x == -1)
                {
                    vertices[vertexID].bonesIndices.x = i;
                    vertices[vertexID].weights.x = weight;
                }
                else if (vertices[vertexID].bonesIndices.y == -1)
                {
                    vertices[vertexID].bonesIndices.y = i;
                    vertices[vertexID].weights.y = weight;
                }
                else if (vertices[vertexID].bonesIndices.z == -1)
                {
                    vertices[vertexID].bonesIndices.z = i;
                    vertices[vertexID].weights.z = weight;
                }
            }
        }
    }
    indices.resize(mesh->mNumFaces * 3);
    for (uint32_t i = 0; i < indices.size(); i += 3)
    {
        indices[i + 0] = mesh->mFaces[i / 3].mIndices[0];
        indices[i + 1] = mesh->mFaces[i / 3].mIndices[1];
        indices[i + 2] = mesh->mFaces[i / 3].mIndices[2];
    }

    //TODO import animations as well
    LOG("Imported vertices & indices data successfully");
    importer.FreeScene();
}