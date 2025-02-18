#include "KitModelResourceCache.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Kitsune
{
    KitMeshData ProcessMesh(aiMesh *mesh, const aiScene *scene)
    {
        std::vector<KitVertex> vertices;
        std::vector<uint32_t>  indices;

        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            KitVertex vertex{};
            // process vertex positions, normals and texture coordinates
            glm::vec3 vector;
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.position = vector;

            // color
            if (mesh->mColors[0] != nullptr)
            {
                vertex.color.r = mesh->mColors[0][i].r;
                vertex.color.g = mesh->mColors[0][i].g;
                vertex.color.b = mesh->mColors[0][i].b;
            }
            else
            {
                vertex.color = {1.f, 1.f, 1.f};
            }

            // normal
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;

            // uv
            if(mesh->mTextureCoords[0])
            {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.uv = vec;
            }
            else
                vertex.uv = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }

        // process indices
        for(unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for(unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        return KitMeshData(vertices, indices);
    }

    void KitModelResourceCache::ProcessNode(aiNode *node, const aiScene *scene, KitModel* model)
    {
        // process all the node's meshes (if any)
        for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            model->AddMesh(device_, ProcessMesh(mesh, scene));
        }

        // then do the same for each of its children
        for(unsigned int i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene, model);
        }
    }

    bool KitModelResourceCache::LoadFromFile(const std::string& name, const std::string& file_path)
    {
        Assimp::Importer import;
        const aiScene *scene = import.ReadFile(file_path, aiProcess_Triangulate);

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            return false;
        }

        auto model = std::make_unique<KitModel>();

        ProcessNode(scene->mRootNode, scene, model.get());

        cache_[name] = std::move(model);

        return true;
    }
} // Kitsune