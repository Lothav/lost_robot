//
// Created by luiz0tavio on 6/4/18.
//

#ifndef MOONRAT_CAMERA_HPP
#define MOONRAT_CAMERA_HPP

#include <GL/glew.h>
#include <iostream>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <array>
#include "Object3D.hpp"

namespace Renderer
{
    class Camera
    {

    private:

        glm::mat4 mvp_;

        glm::mat4 model_;
        glm::mat4 view_;
        glm::mat4 projection_;
        glm::vec3 center_;
        glm::vec3 origin_;

        GLuint shader_view_pos_;

        std::array<int, 2> window_size_;

    public:
        Camera(GLuint shader_program, std::array<int, 2> window_size) : shader_view_pos_(0), window_size_(window_size)
        {
            auto loc = glGetUniformLocation(shader_program, "mvp");
            if (loc < 0) std::cerr << "Can't find 'view' uniform on shader!" << std::endl;
            this->shader_view_pos_ = static_cast<GLuint>(loc);

            // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
            this->projection_ = glm::perspective(glm::radians(45.0f), (GLfloat)window_size[0] / (GLfloat)window_size[1], 0.1f, 100.0f);

            this->center_ = glm::vec3( 0, 0, 6 );
            this->origin_ = glm::vec3( 0, 0,  0 );

            this->view_ = glm::lookAt(
                this->center_,        // Camera is at (4,3,3), in World Space
                this->origin_,        // and looks at the origin
                glm::vec3( 0, 1, 0 )  // Head is up (set to 0,-1,0 to look upside-down)
            );

            this->model_ = glm::mat4(1.f);

            this->updateMVP();
        }

        void update(Renderer::Object3D *object)
        {
            this->model_ = object->getModelMatrix();
            this->updateMVP();
            glUniformMatrix4fv(this->shader_view_pos_, 1, GL_FALSE, &this->mvp_[0][0]);
        }

        void move(glm::vec3 direction)
        {
            this->view_ = glm::translate(this->view_, direction);

            this->updateMVP();
        }

        std::array<int, 2> getWindowSize()
        {
            return this->window_size_;
        };

    private:

        void updateMVP()
        {
            this->mvp_ = this->projection_ * this->view_ * this->model_;
        }

    };
}



#endif //MOONRAT_CAMERA_HPP
