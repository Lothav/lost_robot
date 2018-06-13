//
// Created by luiz0tavio on 6/10/18.
//

#ifndef MOONRAT_OBJECT3D_HPP
#define MOONRAT_OBJECT3D_HPP

#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/cexport.h>
#include <GL/glew.h>
#include <array>
#include <vector>

struct Mesh {
    std::vector<std::array<GLfloat, 3>> vertices;
};

namespace Renderer
{
    class Object3D
    {

    private:

        std::vector<Mesh *> meshes_;
        GLuint vbo_;

    public:

        Object3D(const std::string& file_path)
        {
            Assimp::Importer Importer;
            const aiScene* pScene = Importer.ReadFile(file_path.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

            if (pScene && pScene->HasMeshes()) {

                for (int i = 0; i < pScene->mNumMeshes; i++) {

                    auto mesh = pScene->mMeshes[i];

                    auto meshObj = new Mesh{};
                    meshObj->vertices = {};
                    for (int j = 0; j < mesh->mNumVertices; j++) {
                        meshObj->vertices.push_back({mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z});
                    }
                    this->meshes_.push_back(meshObj);
                }
            } else {
                printf("Error parsing '%s': '%s'\n", file_path.c_str(), Importer.GetErrorString());
            }
            glGenBuffers(1, &this->vbo_);
        }

        std::vector<Mesh *> getMeshes()
        {
            return this->meshes_;
        }

        GLuint getVBO()
        {
            return this->vbo_;
        }

    };
}



#endif //MOONRAT_OBJECT3D_HPP
