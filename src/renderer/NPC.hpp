//
// Created by ivan on 16/06/18.
//

#ifndef MOONRAT_NPC_HPP
#define MOONRAT_NPC_HPP


#include "Object3D.hpp"
#include "Player.hpp"
#include <glm/gtx/norm.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace Renderer {
    class NPC : public Object3D {
        
    private:
        GLfloat turn_angle_ = 0.0f;
        
    public:
        NPC(const glm::vec3 &position) : Object3D(position) {}

        void timeTick(Renderer::Player *player)
        {
            const float speed = .25;
            const float threshold = .25;

            auto a = getPosition();
            auto b = player->getPosition();

            auto delta = b - a;

            if (glm::l2Norm(delta) > threshold) {
                move(speed * glm::normalize(delta));
            }
            turn_angle_ = glm::degrees(glm::angle(glm::normalize(delta), glm::vec3(0.0f, 1.0f, 0.0f)));
            if (delta.x > 0) turn_angle_ *= -1;
        }

        glm::mat4 getModelMatrix() override
        {
            return directionRotation(Object3D::getModelMatrix());
        }

        private:
            glm::mat4 directionRotation(glm::mat4 model_) {
                return glm::rotate(model_, glm::radians(turn_angle_), glm::vec3(0.0f, 0.0f, 1.0f));
            }
    };
}


#endif //MOONRAT_NPC_HPP
