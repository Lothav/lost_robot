//
// Created by ivan on 15/06/18.
//

#ifndef MOONRAT_PLAYER_H
#define MOONRAT_PLAYER_H

#include "Object3D.hpp"

namespace Renderer {

class Player : public Object3D {
public:
    enum class Direction {
        UP, LEFT, DOWN, RIGHT
    };

private:
    Direction direction_ = Direction::UP;

public:
    void turn(Direction d) {
        direction_ = d;
    }

    virtual glm::mat4 getModelMatrix() override {
        float angle;

        auto model = Object3D::getModelMatrix();

        switch (direction_) {
            case Direction::UP:
                angle = 0;
                break;

            case Direction::LEFT:
                angle = glm::radians(90.0f);
                break;

            case Direction::DOWN:
                angle = glm::radians(180.0f);
                break;

            case Direction::RIGHT:
                angle = glm::radians(270.0f);
                break;
        }

        return directionRotation(model);
    }

    void move(glm::vec3 direction) override
    {
        int s = 100;
        auto d = directionRotation(glm::mat4(1.0f)) * glm::vec4(s * direction.x, s * direction.y, 0.0f, 1.0f);;
        position_ += glm::vec3(d.x, d.y, d.z);
    }

private:
    glm::mat4 directionRotation(glm::mat4 model_) {
        float angle;

        switch (direction_) {
            case Direction::UP:
                angle = 0;
                break;

            case Direction::LEFT:
                angle = glm::radians(90.0f);
                break;

            case Direction::DOWN:
                angle = glm::radians(180.0f);
                break;

            case Direction::RIGHT:
                angle = glm::radians(270.0f);
                break;
        }

        return glm::rotate(model_, angle, glm::vec3(0.0f, 0.0f, 1.0f));
    }
};

}


#endif //MOONRAT_PLAYER_H
