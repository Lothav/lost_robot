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
#include <glm/glm.hpp>

struct Mesh {
    std::vector<std::array<GLfloat, 5>> vertices;
    GLuint texture_index;
};

namespace Renderer {
    class Object3D {

    private:

        std::vector<Mesh *> meshes_;
        GLuint vbo_;
        std::vector<GLuint> textures_;
        glm::mat4 modelMatrix;

    public:

        Object3D() : modelMatrix(glm::mat4(1.0f)) {
            glGenBuffers(1, &this->vbo_);
        }

        void addMesh(Mesh *mesh) {
            this->meshes_.push_back(mesh);
        }

        void importFromFile(const std::string &file_path) {
            Assimp::Importer Importer;
            const aiScene *pScene = Importer.ReadFile(file_path.c_str(),
                                                      aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                                                      aiProcess_FlipUVs);

            if (pScene) {

                if (pScene->HasMeshes()) {

                    for (int i = 0; i < pScene->mNumMeshes; i++) {

                        auto mesh = pScene->mMeshes[i];

                        auto meshObj = new Mesh{};
                        meshObj->vertices = {};
                        meshObj->texture_index = mesh->mMaterialIndex;

                        for (int k = 0; k < mesh->mNumFaces; k++) {
                            auto face = mesh->mFaces[k];
                            for (int j = 0; j < face.mNumIndices; ++j) {
                                auto pos = mesh->mVertices[face.mIndices[j]];
                                auto uv  = mesh->mTextureCoords[0][face.mIndices[j]];
                                meshObj->vertices.push_back({pos.x, pos.y, pos.z, uv.x, uv.y});
                            }
                        }
                        this->addMesh(meshObj);
                    }
                }

                if (pScene->HasMaterials()) {
                    for (int i = 0; i < pScene->mNumMaterials; i++) {
                        aiString path;
                        auto material = pScene->mMaterials[i];
                        if (aiReturn_SUCCESS == aiGetMaterialTexture(material, aiTextureType_DIFFUSE, 0, &path)) {
                            this->loadTexture(std::string("./data/mobs/spider/") + path.data, i== 1 ? GL_RGB : GL_RGBA);
                        }
                    }
                }

            } else {
                printf("Error parsing '%s': '%s'\n", file_path.c_str(), Importer.GetErrorString());
            }

        }

        void loadTexture(const std::string &path, const GLenum format) {
            SDL_Surface *surf = IMG_Load(path.c_str());

            if (surf == nullptr) {
                std::cerr << "Err loading texture file: " << path << std::endl;
                return;
            }

            GLuint id;
            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);
            glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, surf->w, surf->h);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, surf->w, surf->h, format, GL_UNSIGNED_BYTE, surf->pixels);

            this->textures_.push_back(id);

            free(surf);

            unsigned int error_code = glGetError();
            if (error_code != GL_NO_ERROR) {
                std::cerr << "OpenGL error glTexImage2D. Error code: " << std::to_string(error_code) << std::endl;
            }

            glGenerateMipmap(GL_TEXTURE_2D);
        }

        std::vector<GLuint> getTextures() const {
            return this->textures_;
        }

        std::vector<Mesh *> getMeshes() const {
            return this->meshes_;
        }

        void setModelMatrix(glm::mat4 view) {
            this->modelMatrix = view;
        }

        glm::mat4 getModelMatrix() {
            return this->modelMatrix;
        }

        GLuint getVBO() const {
            return this->vbo_;
        }

    };
}


#endif //MOONRAT_OBJECT3D_HPP
