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
        glm::mat4 view_rotation_;

        glm::mat4 projection_;

        glm::vec3 initial_pos_;

        glm::vec3 player_;
        glm::vec3 pos_;
        glm::vec3 target_;
        glm::vec3 up_;

        GLfloat step_size_;

        GLuint shader_view_pos_;

        std::array<int, 2> window_size_;

        std::array<int, 2> mouse_pos_;

    public:
        Camera(GLuint shader_program, std::array<int, 2> window_size) :
                shader_view_pos_(0),
                window_size_(window_size)
        {
            auto loc = glGetUniformLocation(shader_program, "mvp");
            if (loc < 0) std::cerr << "Can't find 'view' uniform on shader!" << std::endl;
            this->shader_view_pos_ = static_cast<GLuint>(loc);

            // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
            this->projection_ = glm::perspective(glm::radians(45.0f), (GLfloat)window_size[0] / (GLfloat)window_size[1], 0.1f, 100.0f);

            initial_pos_ = glm::vec3( 0.0f,  0.0f, 3.0f );

            this->pos_ = initial_pos_;
            this->target_ = glm::vec3( 0,  0,   0 );
            this->up_ = glm::vec3( 0, 1, 0 );

            step_size_ = 0.01;

            updateView();

            this->view_rotation_ = glm::mat4(1.f);

            this->model_ = glm::mat4(1.f);

            mouse_pos_[0] = window_size[0] / 2;
            mouse_pos_[1] = window_size[1] / 2;

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
            player_ = target;

            pos_ = initial_pos_ + player_;
            adjustRotation();
            updateView();
            updateMVP();
        }



        void rotate(int x, int y)
        {
            mouse_pos_[0] = x;
            mouse_pos_[1] = y;
            updateView();
            updateMVP();
        }

        void adjustRotation()
        {
            float x_n =  (float) mouse_pos_[0] / window_size_[0];
            float y_n = (float) mouse_pos_[1] / window_size_[1];

            float angle_h = glm::radians((x_n - 0.5f) * 90);
            float angle_v = glm::radians((0.5f - y_n) * 90);

            glm::vec3 direction(
                cos(angle_v) * sin(angle_h),
                sin(angle_v),
                cos(angle_v) * cos(angle_h)
            );

            glm::vec3 right = glm::vec3(
                sin(angle_h - 3.14f/2.0f),
                0,
                cos(angle_h - 3.14f/2.0f)
            );

            glm::vec3 up = glm::cross( right, direction );

            target_ = direction + player_;
            up_ = up;
        }

        std::array<int, 2> getWindowSize()
        {
            return this->window_size_;
        };

    private:
        void updateView()
        {
            adjustRotation();

            this->view_ = glm::lookAt(
                pos_, // Center
                target_,
                up_  // Head up
            );
        }

        void updateMVP()
        {
            this->mvp_ = this->projection_ * this->view_ * this->model_;
        }

        glm::vec3 rotateVector(float angle, glm::vec3 axis, glm::vec3 vec) {
            auto v = glm::rotate(glm::mat4(1.0f), angle, axis) * glm::vec4(vec, 0.0f);
            return glm::vec3(v);
        }

    };
}



#endif //MOONRAT_CAMERA_HPP
