//
// Created by ivan on 15/06/18.
//

#ifndef MOONRAT_PLAYER_H
#define MOONRAT_PLAYER_H

#include "Object3D.hpp"

namespace Renderer {

class Player : public Object3D {

private:

    GLfloat turn_angle_ = .0f;

public:

    void turn(GLfloat angle) {
        turn_angle_ = angle;
    }

    glm::mat4 getModelMatrix() override {
        return directionRotation(Object3D::getModelMatrix());
    }

    void move(glm::vec3 direction) override
    {
        //int s = 100;
        //auto d = directionRotation(glm::mat4(1.0f)) * glm::vec4(s * direction.x, s * direction.y, 0.0f, 1.0f);;
        position_ += glm::vec3(direction.x, direction.y, direction.z);
    }

private:

    glm::mat4 directionRotation(glm::mat4 model_) {
        return glm::rotate(model_, glm::radians(turn_angle_), glm::vec3(0.0f, 0.0f, 1.0f));
    }
};

}


#endif //MOONRAT_PLAYER_H
