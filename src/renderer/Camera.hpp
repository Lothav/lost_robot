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
#include <glm/gtc/type_ptr.hpp>

namespace Renderer
{

    class Camera
    {

    private:

        glm::mat4 model_;
        glm::mat4 view_;
        glm::mat4 projection_;

        glm::vec3 initial_pos_;
        glm::vec3 player_pos_;
        glm::vec3 pos_;
        glm::vec3 target_;
        glm::vec3 up_;

        GLuint shader_model_pos_;
        GLuint shader_view_pos_;
        GLuint shader_projection_pos_;
        GLuint shader_eye_pos_;

        std::array<int, 2> window_size_;

    public:

        Camera(GLuint shader_program, std::array<int, 2> window_size, glm::vec3 player_pos) :
                window_size_(window_size),
                player_pos_(player_pos)
        {
            auto loc = glGetUniformLocation(shader_program, "model");
            if (loc < 0) std::cerr << "Can't find 'model' uniform on shader!" << std::endl;
            this->shader_model_pos_ = static_cast<GLuint>(loc);

            loc = glGetUniformLocation(shader_program, "view");
            if (loc < 0) std::cerr << "Can't find 'view' uniform on shader!" << std::endl;
            this->shader_view_pos_ = static_cast<GLuint>(loc);

            loc = glGetUniformLocation(shader_program, "projection");
            if (loc < 0) std::cerr << "Can't find 'projection' uniform on shader!" << std::endl;
            this->shader_projection_pos_ = static_cast<GLuint>(loc);

            loc = glGetUniformLocation(shader_program, "eye");
            if (loc < 0) std::cerr << "Can't find 'eye' uniform on shader!" << std::endl;
            this->shader_eye_pos_ = static_cast<GLuint>(loc);

            // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
            this->projection_   = glm::perspective(glm::radians(45.0f), (GLfloat)window_size[0] / (GLfloat)window_size[1], 0.1f, 100.0f);

            this->initial_pos_  = glm::vec3( .0f, -1.25f, 1.2f );
            this->pos_          = initial_pos_ + player_pos;

            this->up_           = glm::vec3(0, 1, 0);
            this->model_        = glm::mat4(1.f);

            this->adjustRotation();
            this->updateView();
        }

        void update(Renderer::Object3D *object)
        {
            this->model_ = object->getModelMatrix();
            glUniformMatrix4fv(this->shader_model_pos_,      1, GL_FALSE, glm::value_ptr(this->model_));
            glUniformMatrix4fv(this->shader_view_pos_,       1, GL_FALSE, glm::value_ptr(this->view_));
            glUniformMatrix4fv(this->shader_projection_pos_, 1, GL_FALSE, glm::value_ptr(this->projection_));
            glUniform3fv(this->shader_eye_pos_,              1, glm::value_ptr(this->pos_));
        }

        void move(glm::vec3 target)
        {
            this->player_pos_ = target;

            this->pos_ = this->initial_pos_ + this->player_pos_;
            this->adjustRotation();
            this->updateView();
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

    };
}



#endif //MOONRAT_CAMERA_HPP
