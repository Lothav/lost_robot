//
// Created by ivan on 16/06/18.
//

#include "Interactions.hpp"

Renderer::Interactions &Renderer::Interactions::getInstance() {
    static Renderer::Interactions instance;
    return instance;
}
