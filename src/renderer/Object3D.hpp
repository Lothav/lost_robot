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
#include <iostream>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_image.h>

struct Mesh {
    std::vector<std::array<GLfloat, 5>> vertices;
    unsigned int texture_index;
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
            const aiScene *pScene = Importer.ReadFile(file_path.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

            if (pScene) {

                if (pScene->HasMeshes()) {
                    for (int i = 0; i < pScene->mNumMeshes; i++) {

                        auto mesh = pScene->mMeshes[i];

                        std::cout << "Channels: " << std::to_string(mesh->GetNumUVChannels()) << std::endl;
                       // std::cout << std::to_string(mesh->mNumUVComponents) << std::endl;

                        auto meshObj = new Mesh{};
                        meshObj->vertices = {};
                        meshObj->texture_index = mesh->mMaterialIndex;
                        for (int j = 0; j < mesh->mNumVertices; j++) {
                            auto vertex = mesh->mVertices[j];
                            auto uv = mesh->mTextureCoords[0][j];
                            meshObj->vertices.push_back({vertex.x, vertex.y, vertex.z, uv.x, uv.y});
                        }
                        this->meshes_.push_back(meshObj);
                    }

                    if (pScene->HasMaterials()) {
                        for (int i = 0; i < pScene->mNumMaterials; i++) {
                            aiString path;
                            auto material = pScene->mMaterials[i];
                            if (aiReturn_SUCCESS == aiGetMaterialTexture(material, aiTextureType_DIFFUSE, 0, &path)) {
                                this->loadTexture(std::string("./data/mobs/spider/") + path.data, GL_RGBA);
                            }
                        }
                    }

                }
            } else {
                printf("Error parsing '%s': '%s'\n", file_path.c_str(), Importer.GetErrorString());
            }

            glGenBuffers(1, &this->vbo_);
        }

        void loadTexture(const std::string& path, const GLenum format)
        {
            SDL_Surface* surf = IMG_Load(path.c_str());

            GLuint id;
            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);
            glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, surf->w, surf->h);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0 ,0, surf->w, surf->h, format, GL_UNSIGNED_BYTE, surf->pixels);

            std::cout << std::to_string(id) << std::endl;

            free(surf);

            unsigned int error_code = glGetError();
            if (error_code != GL_NO_ERROR) {
                std::cerr << "OpenGL error glTexImage2D. Error code: " << std::to_string(error_code) << std::endl;
            }

            glGenerateMipmap(GL_TEXTURE_2D);
        }

        std::vector<Mesh *> getMeshes() const
        {
            return this->meshes_;
        }

        GLuint getVBO() const
        {
            return this->vbo_;
        }

    };
}


#endif //MOONRAT_OBJECT3D_HPP
