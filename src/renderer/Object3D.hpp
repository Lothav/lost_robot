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
#include <map>

struct Vertex {
    std::vector<std::array<GLfloat, 3>> position;
    std::vector<std::array<GLfloat, 2>> uv;
    std::vector<std::array<GLfloat, 3>> normal;
};

struct Mesh {
    std::vector<std::array<GLfloat, 5>> vertices;
    GLuint texture_index;
};

struct BoneInfo
{
    aiMatrix4x4 BoneOffset;
    aiMatrix4x4 FinalTransformation;

    BoneInfo()
    {
        aiMatrix4x4 zero = {
            0.f, 0.f, 0.f, 0.f,
            0.f, 0.f, 0.f, 0.f,
            0.f, 0.f, 0.f, 0.f,
            0.f, 0.f, 0.f, 0.f,
        };

        BoneOffset = zero;
        FinalTransformation = zero;
    }
};

namespace Renderer {
    class Object3D {

    private:

        std::vector<Mesh *> meshes_;
        GLuint vbo_;
        std::vector<GLuint> textures_;
        glm::mat4 model_;

    public:

        Object3D() : model_(glm::mat4(1.0f)) {
            glGenBuffers(1, &this->vbo_);
        }

        void addMesh(Mesh *mesh) {
            this->meshes_.push_back(mesh);
        }

        std::vector<BoneInfo> m_BoneInfo;

        std::map<std::string,uint> m_BoneMapping; // maps a bone name to its index

        uint m_NumBones = 0;

        uint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
        {
            for (uint i = 0 ; i < pNodeAnim->mNumPositionKeys - 1 ; i++) {
                if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
                    return i;
                }
            }

            assert(0);

            return 0;
        }


        uint FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
        {
            assert(pNodeAnim->mNumRotationKeys > 0);

            for (uint i = 0 ; i < pNodeAnim->mNumRotationKeys - 1 ; i++) {
                if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
                    return i;
                }
            }

            assert(0);

            return 0;
        }


        uint FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
        {
            assert(pNodeAnim->mNumScalingKeys > 0);

            for (uint i = 0 ; i < pNodeAnim->mNumScalingKeys - 1 ; i++) {
                if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
                    return i;
                }
            }

            assert(0);

            return 0;
        }


        void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
        {
            if (pNodeAnim->mNumPositionKeys == 1) {
                Out = pNodeAnim->mPositionKeys[0].mValue;
                return;
            }

            uint PositionIndex = FindPosition(AnimationTime, pNodeAnim);
            uint NextPositionIndex = (PositionIndex + 1);
            assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
            float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
            float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
            assert(Factor >= 0.0f && Factor <= 1.0f);
            const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
            const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
            aiVector3D Delta = End - Start;
            Out = Start + Factor * Delta;
        }


        void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
        {
            // we need at least two values to interpolate...
            if (pNodeAnim->mNumRotationKeys == 1) {
                Out = pNodeAnim->mRotationKeys[0].mValue;
                return;
            }

            uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
            uint NextRotationIndex = (RotationIndex + 1);
            assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
            float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
            float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
            assert(Factor >= 0.0f && Factor <= 1.0f);
            const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
            const aiQuaternion& EndRotationQ   = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
            aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
            Out = Out.Normalize();
        }


        void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
        {
            if (pNodeAnim->mNumScalingKeys == 1) {
                Out = pNodeAnim->mScalingKeys[0].mValue;
                return;
            }

            uint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
            uint NextScalingIndex = (ScalingIndex + 1);
            assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
            float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
            float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
            assert(Factor >= 0.0f && Factor <= 1.0f);
            const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
            const aiVector3D& End   = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
            aiVector3D Delta = End - Start;
            Out = Start + Factor * Delta;
        }

        const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string& NodeName)
        {
            for (uint i = 0 ; i < pAnimation->mNumChannels ; i++) {
                const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

                if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
                    return pNodeAnim;
                }
            }

            return NULL;
        }

        void LoadBones(uint MeshIndex, const aiMesh* pMesh, vector<VertexBoneData>& Bones)
        {
            for (uint i = 0 ; i < pMesh->mNumBones ; i++) {
                uint BoneIndex = 0;
                std::string BoneName(pMesh->mBones[i]->mName.data);

                if (m_BoneMapping.find(BoneName) == m_BoneMapping.end()) {
                    // Allocate an index for a new bone
                    BoneIndex = m_NumBones;
                    m_NumBones++;
                    BoneInfo bi;
                    m_BoneInfo.push_back(bi);
                    m_BoneInfo[BoneIndex].BoneOffset = pMesh->mBones[i]->mOffsetMatrix;
                    m_BoneMapping[BoneName] = BoneIndex;
                }
                else {
                    BoneIndex = m_BoneMapping[BoneName];
                }

                for (uint j = 0 ; j < pMesh->mBones[i]->mNumWeights ; j++) {
                    uint VertexID = m_Entries[MeshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
                    float Weight  = pMesh->mBones[i]->mWeights[j].mWeight;
                    Bones[VertexID].AddBoneData(BoneIndex, Weight);
                }
            }
        }


        void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const aiMatrix4x4& ParentTransform, const aiScene *pScene)
        {
            std::string NodeName = pNode->mName.data;

            const aiAnimation* pAnimation = pScene->mAnimations[0];

            aiMatrix4x4 NodeTransformation(pNode->mTransformation);

            const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

            if (pNodeAnim) {
                // Interpolate scaling and generate scaling transformation matrix
                aiVector3D Scaling;
                CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
                aiMatrix4x4 ScalingM;
                aiMatrix4x4::Scaling({Scaling.x, Scaling.y, Scaling.z}, ScalingM);

                // Interpolate rotation and generate rotation transformation matrix
                aiQuaternion RotationQ;
                CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
                aiMatrix4x4 RotationM = aiMatrix4x4(RotationQ.GetMatrix());

                // Interpolate translation and generate translation transformation matrix
                aiVector3D Translation;
                CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
                aiMatrix4x4 TranslationM;
                aiMatrix4x4::Translation({Translation.x, Translation.y, Translation.z}, TranslationM);

                // Combine the above transformations
                NodeTransformation = TranslationM * RotationM * ScalingM;
            }

            aiMatrix4x4 GlobalTransformation = ParentTransform * NodeTransformation;

            if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
                uint BoneIndex = m_BoneMapping[NodeName];
                auto transf = pScene->mRootNode->mTransformation;
                transf.Inverse();
                m_BoneInfo[BoneIndex].FinalTransformation = transf * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
            }

            for (uint i = 0 ; i < pNode->mNumChildren ; i++) {
                ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation, pScene);
            }
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


                        LoadBones(i, mesh, );

                        auto TimeInSeconds = SDL_GetTicks() / 1000;

                        aiMatrix4x4 Identity;

                        auto TicksPerSecond = (float)(pScene->mAnimations[0]->mTicksPerSecond != 0 ? pScene->mAnimations[0]->mTicksPerSecond : 25.0f);
                        float TimeInTicks = TimeInSeconds * TicksPerSecond;
                        float AnimationTime = fmod(TimeInTicks, (float)pScene->mAnimations[0]->mDuration);

                        ReadNodeHeirarchy(AnimationTime, pScene->mRootNode, Identity, pScene);

                        std::vector<aiMatrix4x4> Transforms;
                        Transforms.resize(mesh->mNumBones);

                        for (uint i = 0 ; i < mesh->mNumBones; i++) {
                            Transforms[i] = m_BoneInfo[i].FinalTransformation;
                        }

                        for (int l = 0; l < mesh->mNumBones; ++l) {
                            auto bone = mesh->mBones[l];
                            for (int j = 0; j < bone->mNumWeights; ++j) {
                                auto weight = bone->mWeights[j];
                                for (int k = 0; k < meshObj->vertices.size(); ++k) {
                                    //if (weight.mVertexId == meshObj->vertices) {
//
                                    //}
                                }

                            }
                        }

                        this->addMesh(meshObj);
                    }
                }

                if (pScene->HasAnimations()) {
                    for (int i = 0; i < pScene->mNumMeshes; ++i) {
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

        void rotate(float x, float y)
        {
            this->model_ = glm::rotate(this->model_, glm::radians(x), glm::vec3(0.0f, 1.0f, 0.0f));
            this->model_ = glm::rotate(this->model_, glm::radians(y), glm::vec3(1.0f, 0.0f, 0.0f));
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

        glm::mat4 getModelMatrix() {
            return this->model_;
        }

        GLuint getVBO() const {
            return this->vbo_;
        }

    };
}


#endif //MOONRAT_OBJECT3D_HPP
