// Created by ivan on 15/06/18.
//

#ifndef MOONRAT_PLAYER_H
#define MOONRAT_PLAYER_H

#include "Object3D.hpp"

const float PLAYER_SCALE = 0.025f;

namespace Renderer {

    class Player : public Object3D {

    private:

        GLfloat turn_angle_ = .0f;
        GLfloat z_ = 0.0f;
        unsigned int lives = 5;
        unsigned int points = 0;

    public:

        Player(const glm::vec3 &position) : Object3D(position) {}

        void turn(GLfloat angle) {
            turn_angle_ = angle;
        }

        glm::mat4 getModelMatrix() override
        {
            return directionRotation(
                    glm::translate(
                            Object3D::getModelMatrix(),
                            glm::vec3(0.0f, 0.0f, z_)
                    )
            );
        }

        unsigned int getLives()
        {
            return this->lives;
        }

        std::string getPointsText()
        {
            std::string prefix;

            if (this->points / 10000.f < 1.f){
                prefix += "0";
            }
            if (this->points / 1000.f < 1.f){
                prefix += "0";
            }
            if (this->points / 100.f < 1.f){
                prefix += "0";
            }
            if (this->points / 10.f < 1.f){
                prefix += "0";
            }

            return prefix + std::to_string(this->points);
        }

        void move(glm::vec3 direction) override
        {
            position_ += direction;
        }

    private:

        glm::mat4 directionRotation(glm::mat4 model_) {
            return glm::rotate(model_, glm::radians(turn_angle_), glm::vec3(0.0f, 0.0f, 1.0f));
        }
    };

}


#endif //MOONRAT_PLAYER_H
