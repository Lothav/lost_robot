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

            this->pos_ = glm::vec3( 0.0f,  0.0f, 3.0f );
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

        enum class Direction {
            UP, DOWN, LEFT, RIGHT
        };

        void move(glm::vec3 direction)
        {
            this->view_ = glm::translate(view_, direction);
            updateMVP();
        }

        const static int MARGIN = 10;

        void rotate(int x, int y)
        {
            float x_n =  (float) x / window_size_[0];
            float y_n = (float) y / window_size_[1];

            view_rotation_ = glm::rotate(glm::mat4(1.0f), glm::radians((0.5f - x_n) * 90), glm::vec3(0.0f, 1.0f, 0.0f));
            view_rotation_ = glm::rotate(view_rotation_, glm::radians((0.5f - y_n) * 90), glm::vec3(1.0f, 0.0f, 0.0f));

            updateMVP();
        }

        std::array<int, 2> getWindowSize()
        {
            return this->window_size_;
        };

    private:
        void updateView()
        {
            this->view_ = glm::lookAt(
                    this->pos_, // Center
                    this->target_, // Look
                    this->up_  // Head up
            );
        }

        void updateMVP()
        {
            this->mvp_ = this->projection_  *  this->view_ * this->view_rotation_ * this->model_;
        }

    };
}



#endif //MOONRAT_CAMERA_HPP
