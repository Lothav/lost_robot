//
// Created by luiz0tavio on 6/2/18.
//

#ifndef BREAKOUT_BULKOBJECT3D_HPP
#define BREAKOUT_BULKOBJECT3D_HPP

#include <memory>
#include "Object3D.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include <glm/mat4x4.hpp> // glm::vec4

namespace Renderer
{
    class BulkObject3D
    {
    public:

        static BulkObject3D& getInstance();

        BulkObject3D(BulkObject3D const&) = delete;
        void operator=(BulkObject3D const&)  = delete;

    private:

        std::vector<Object3D *, Memory::Allocator<Object3D *> > objects3d_;

        Renderer::Shader* shader_;
        GLuint shader_tex_pos_;
        GLuint shader_uv_pos_;
        GLuint shader_vert_pos_;

        BulkObject3D();

    public:

        void * operator new (std::size_t size);

        void  operator delete (void* ptr, std::size_t) {
        }

        void push_back(Object3D* Object3D);


        void remove(Object3D *Object3D);

        void draw(Renderer::Camera* camera);

        GLuint GetShaderProgram()
        {
            return this->shader_->getShaderProgram();
        }
    };
}

#endif //BREAKOUT_BULKOBJECT3D_HPP
