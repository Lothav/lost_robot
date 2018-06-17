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

#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define MAX_BONES 100

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
        ZERO_MEM(&BoneOffset);
        ZERO_MEM(&FinalTransformation);
    }
};

#define NUM_BONES_PER_VEREX 4

struct VertexBoneData
{
    uint IDs[NUM_BONES_PER_VEREX];
    float Weights[NUM_BONES_PER_VEREX];

    VertexBoneData()
    {
        Reset();
    };

    void Reset()
    {
        ZERO_MEM(IDs);
        ZERO_MEM(Weights);
    }

    void AddBoneData(uint BoneID, float Weight){
        for (uint i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(IDs) ; i++) {
            if (Weights[i] == 0.0) {
                IDs[i]     = BoneID;
                Weights[i] = Weight;
                return;
            }
        }

        // should never get here - more bones than we have space for
        assert(0);
    }
};
#define INVALID_MATERIAL 0xFFFFFFFF

namespace Renderer {
    class Object3D {

    private:

        std::vector<Mesh *> meshes_;
        GLuint vbo_;
        std::vector<GLuint> textures_;
        glm::mat4 model_;

    public:

        Object3D() : model_(glm::mat4(1.0f))
        {
            glGenBuffers(1, &this->vbo_);
        }

        void addMesh(Mesh *mesh) {
            this->meshes_.push_back(mesh);
        }

        GLuint m_boneLocation[MAX_BONES];

        const aiScene *pScene;

        std::vector<BoneInfo> m_BoneInfo;

        std::map<std::string,uint> m_BoneMapping; // maps a bone name to its index

        uint m_NumBones = 0;

        struct MeshEntry {
            MeshEntry()
            {
                NumIndices    = 0;
                BaseVertex    = 0;
                BaseIndex     = 0;
                MaterialIndex = INVALID_MATERIAL;
            }

            unsigned int NumIndices;
            unsigned int BaseVertex;
            unsigned int BaseIndex;
            unsigned int MaterialIndex;
        };

        std::vector<MeshEntry> m_Entries;

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


        void InitMesh(uint MeshIndex, const aiMesh* paiMesh, std::vector<VertexBoneData>& Bones)
        {
            LoadBones(MeshIndex, paiMesh, Bones);
        }

        bool InitFromScene(const aiScene* pScene, GLuint shader_program)
        {
            m_Entries.resize(pScene->mNumMeshes);

            std::vector<glm::vec3> Positions;
            std::vector<VertexBoneData> Bones;
            std::vector<uint> Indices;

            uint NumVertices = 0;
            uint NumIndices = 0;

            // Count the number of vertices and indices
            for (uint i = 0 ; i < m_Entries.size() ; i++) {
                m_Entries[i].BaseVertex    = NumVertices;
                NumVertices += pScene->mMeshes[i]->mNumVertices;
            }

            // Reserve space in the vectors for the vertex attributes and indices
            Bones.resize(NumVertices);

            // Initialize the meshes in the scene one by one
            for (uint i = 0 ; i < m_Entries.size() ; i++) {
                const aiMesh* paiMesh = pScene->mMeshes[i];
                InitMesh(i, paiMesh, Bones);
            }

            GLuint bones_buffer;

            glGenBuffers(1, &bones_buffer);
            glBindBuffer(GL_ARRAY_BUFFER, bones_buffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Bones[0]) * Bones.size(), &Bones[0], GL_STATIC_DRAW);

            auto loc = glGetAttribLocation(shader_program, "boneIDs");
            if (loc < 0) std::cerr << "Can't find 'boneIDs' uniform on shader!" << std::endl;
            auto bone_pos = static_cast<GLuint>(loc);
            glEnableVertexAttribArray(bone_pos);
            glVertexAttribIPointer(bone_pos, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);

            loc = glGetAttribLocation(shader_program, "weights");
            if (loc < 0) std::cerr << "Can't find 'weight' uniform on shader!" << std::endl;
            auto weight_pos = static_cast<GLuint>(loc);
            glEnableVertexAttribArray(weight_pos);
            glVertexAttribPointer(weight_pos , 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);

            return true;
        }

        void LoadBones(uint MeshIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& Bones)
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


        void ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const aiMatrix4x4& ParentTransform, const aiScene *pScene)
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
                ReadNodeHierarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation, pScene);
            }
        }

        void BoneTransform(float TimeInSeconds)
        {
            aiMatrix4x4 Identity;

            std::vector<aiMatrix4x4> Transforms;
            Assimp::Importer Importer;

            auto pScene = Importer.ReadFile(
                    std::string("./data/") + "boblampclean.md5mesh", aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);


            float TicksPerSecond = (float)(pScene->mAnimations[0]->mTicksPerSecond != 0 ? pScene->mAnimations[0]->mTicksPerSecond : 25.0f);
            float TimeInTicks = TimeInSeconds * TicksPerSecond;
            float AnimationTime = fmod(TimeInTicks, (float)pScene->mAnimations[0]->mDuration);

            ReadNodeHierarchy(AnimationTime, pScene->mRootNode, Identity, pScene);

            Transforms.resize(m_NumBones);

            for (uint i = 0 ; i < m_NumBones ; i++) {
                Transforms[i] = m_BoneInfo[i].FinalTransformation;
            }

            for (uint i = 0 ; i < Transforms.size() ; i++) {
                assert(i < MAX_BONES);
                glm::mat4 transform = {
                        Transforms[i].a1, Transforms[i].a2, Transforms[i].a3, Transforms[i].a4,
                        Transforms[i].b1, Transforms[i].b2, Transforms[i].b3, Transforms[i].b4,
                        Transforms[i].c1, Transforms[i].c2, Transforms[i].c3, Transforms[i].c4,
                        Transforms[i].d1, Transforms[i].d2, Transforms[i].d3, Transforms[i].d4,
                };
                glUniformMatrix4fv(m_boneLocation[i], 1, GL_TRUE, &transform[0][0]);
            }
        }

        void importFromFile(const std::string &source_path, const std::string &file_name, const GLuint shader_program) {
            Assimp::Importer Importer;
            auto pScene = Importer.ReadFile(
                    source_path + file_name, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

            if (pScene) {

                InitFromScene(pScene, shader_program);

                for (unsigned int i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(m_boneLocation) ; i++) {
                    char Name[128];

                    memset(Name, 0, sizeof(Name));
                    snprintf(Name, sizeof(Name), "gBones[%d]", i);

                    auto loc = glGetUniformLocation(shader_program, Name);
                    if (loc < 0) std::cerr << "Can't find 'gBones' uniform on shader!" << std::endl;
                    auto bone_pos = static_cast<GLuint>(loc);

                    m_boneLocation[i] = bone_pos;
                }

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
