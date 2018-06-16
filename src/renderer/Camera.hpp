//

#ifndef MOONRAT_CAMERA_HPP
#define MOONRAT_CAMERA_HPP

#include <GL/glew.h>
#include <iostream>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <array>
#include "Object3D.hpp"
#include "Player.hpp"

namespace Renderer
{

    class Camera
    {

    private:

        glm::mat4 mvp_;

        glm::mat4 model_;

        glm::mat4 view_;

        glm::mat4 projection_;

        glm::vec3 initial_pos_;

        glm::vec3 player_pos_;
        glm::vec3 pos_;
        glm::vec3 target_;
        glm::vec3 up_;

        GLuint shader_view_pos_;

        std::array<int, 2> window_size_;

    public:

        Camera(GLuint shader_program, std::array<int, 2> window_size, glm::vec3 player_pos) :
                shader_view_pos_(0),
                window_size_(window_size),
                player_pos_(player_pos)
        {
            auto loc = glGetUniformLocation(shader_program, "mvp");
            if (loc < 0) std::cerr << "Can't find 'view' uniform on shader!" << std::endl;
            this->shader_view_pos_ = static_cast<GLuint>(loc);

            // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
            this->projection_   = glm::perspective(glm::radians(45.0f), (GLfloat)window_size[0] / (GLfloat)window_size[1], 0.1f, 100.0f);

            this->initial_pos_  = glm::vec3( .0f, -1.25f, 1.2f );
            this->pos_          = initial_pos_ + player_pos;

            this->up_           = glm::vec3(0, 1, 0);
            this->model_        = glm::mat4(1.f);

            this->adjustRotation();
            this->updateView();
            this->updateMVP();
        }

        void update(Renderer::Object3D *object)
        {
            this->model_ = object->getModelMatrix();
            this->updateMVP();
            glUniformMatrix4fv(this->shader_view_pos_, 1, GL_FALSE, &this->mvp_[0][0]);
        }

        void move(glm::vec3 target)
        {
            this->player_pos_ = target;

            this->pos_ = this->initial_pos_ + this->player_pos_;
            this->adjustRotation();
            this->updateView();
            this->updateMVP();
        }

        void adjustRotation()
        {
            float angle_h = glm::radians(0.f);
            float angle_v = glm::radians(-0.5f * 90);

            glm::vec3 direction(
                cos(angle_v) * sin(angle_h),
                sin(angle_v),
                cos(angle_v) * cos(angle_h)
            );

            this->target_ = direction + this->player_pos_;
        }

        std::array<int, 2> getWindowSize()
        {
            return this->window_size_;
        };

    private:

        void updateView()
        {
            this->adjustRotation();
            this->view_ = glm::lookAt(pos_, target_, up_);
        }

        void updateMVP()
        {
            this->mvp_ = this->projection_ * this->view_ * this->model_;
        }

    };
}



#endif //MOONRAT_CAMERA_HPP
