//
// Created by ivan on 16/06/18.
//

#ifndef MOONRAT_COLLISIONS_HPP
#define MOONRAT_COLLISIONS_HPP


#include "Player.hpp"

namespace Renderer {
    
class Collisions {

public:
    static bool check(AxisAlignedBB *a, AxisAlignedBB *b) {
        return (
            a->max_.x > b->min_.x &&
            a->min_.x < b->max_.x &&
            a->max_.y > b->min_.y &&
            a->min_.y < b->max_.y &&
            a->max_.z > b->min_.z &&
            a->min_.z < b->max_.z
        );
    }
};

}

#endif //MOONRAT_COLLISIONS_HPP
