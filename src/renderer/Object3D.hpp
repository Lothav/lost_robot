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
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

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

    protected:
        glm::mat4 model_;
        glm::vec3 position_;

    public:

        Object3D() :
                model_(glm::mat4(1.0f)) ,
                position_(glm::vec3(0.0f))
        {
            glGenBuffers(1, &this->vbo_);
        }

        void addMesh(Mesh *mesh) {
            this->meshes_.push_back(mesh);
        }

        void importFromFile(const std::string &source_path, const std::string &file_name) {
            Assimp::Importer Importer;
            const aiScene *pScene = Importer.ReadFile(
                    source_path + file_name, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

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

                            GLuint texture_format = GL_RGB;

                            std::string texture_file_name = path.data;
                            if ((texture_file_name.substr( texture_file_name.length() - 3)) == "png") {
                                texture_format = GL_RGBA;
                            }

                            this->loadTexture(source_path + path.data, texture_format);
                        }
                    }
                }

            } else {
                printf("Error parsing '%s': '%s'\n", source_path + file_name, Importer.GetErrorString());
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

        void transform(glm::mat4 model) {
            this->model_ = model;
        }

        virtual glm::mat4 getModelMatrix() {
            return glm::translate(
                this->model_,
                this->position_
            );
        }

        const glm::vec3 getPosition() const {
            auto p = model_ * glm::vec4(position_, 1.0f);

            return glm::vec3(p.x, p.y, p.z);
        }

        GLuint getVBO() const {
            return this->vbo_;
        }

        virtual void move(glm::vec3 direction)
        {
            position_ += direction;
        }

    };
}


#endif //MOONRAT_OBJECT3D_HPP
