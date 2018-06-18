//
// Created by ivan on 16/06/18.
//

#ifndef MOONRAT_NPC_HPP
#define MOONRAT_NPC_HPP


#include "Object3D.hpp"
#include "Player.hpp"
#include <glm/gtx/norm.hpp>
#include <glm/gtx/vector_angle.hpp>

const int NPC_LIFES = 3;

namespace Renderer
{
    class NPC : public Object3D {

    private:
        GLfloat turn_angle_ = 0.0f;

        int lifes_;

    public:

        NPC(const glm::vec3 &position) : Object3D(position), lifes_(NPC_LIFES) {}

        void turn(GLfloat a)
        {
            turn_angle_ = a;
        }

        glm::mat4 getModelMatrix() override
        {
            return directionRotation(Object3D::getModelMatrix());
        }

        bool shoot() {
            if (lifes_ > 0) lifes_--;
            return lifes_ <= 0;
        }

    private:
        glm::mat4 directionRotation(glm::mat4 model_) {
            return glm::rotate(model_, glm::radians(turn_angle_), glm::vec3(0.0f, 0.0f, 1.0f));
        }
    };
}


#endif //MOONRAT_NPC_HPP
