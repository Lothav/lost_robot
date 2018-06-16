//
// Created by ivan on 16/06/18.
//

#include "Projectiles.hpp"

Renderer::Projectiles &Renderer::Projectiles::getInstance() {
    static Renderer::Projectiles instance;
    return instance;
}
