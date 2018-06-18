//
// Created by luiz0tavio on 6/4/18.
//

#include <glm/gtx/norm.hpp>

#include "Input.hpp"
#include "../renderer/Player.hpp"
#include "../renderer/Interactions.hpp"

Events::Input &Events::Input::getInstance()
{
    static Events::Input instance;
    return instance;
}

bool Events::Input::HandleEvent(
        Renderer::Camera *camera,
        Renderer::Player *player,
        std::function<void()> spawn
)  {

    SDL_Event e;
    while (SDL_PollEvent(&e)) {

        if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_q)) {
            return true;
        }

        if (e.type == SDL_MOUSEMOTION) {
            int mouse_pos_x, mouse_pos_y;
            SDL_GetMouseState(&mouse_pos_x, &mouse_pos_y);
            mouse_angle_ = static_cast<GLfloat>((atan2((camera->getWindowSize()[0]/2) - mouse_pos_x, (camera->getWindowSize()[1]/2) - mouse_pos_y)*180.0000)/3.1416);
        }

        if (e.type == SDL_KEYDOWN) {
            this->key_pressed_[e.key.keysym.sym] = true;

            if (e.key.keysym.sym == SDLK_n) spawn();
        }

        if (e.type == SDL_KEYUP) {
            this->key_pressed_[e.key.keysym.sym] = false;
        }

        if(e.type == SDL_MOUSEBUTTONDOWN) {
            this->key_pressed_[e.button.button] = true;
            if (e.button.button == SDL_BUTTON_LEFT) {
                Renderer::Interactions::getInstance().fire(player);
            }
        }

        if(e.type == SDL_MOUSEBUTTONUP) {
            this->key_pressed_[e.button.button] = false;
        }
    }

    player->turn(mouse_angle_);

    glm::vec3 direction(0.0f);

    if(this->key_pressed_[SDLK_w]) direction += glm::vec3(0.0f, 1.0f, 0.0f);

    if(this->key_pressed_[SDLK_a]) direction += glm::vec3(-1.0f, 0.0f, 0.0f);

    if(this->key_pressed_[SDLK_s]) direction += glm::vec3(0.0f, -1.0f, 0.0f);

    if(this->key_pressed_[SDLK_d]) direction += glm::vec3(1.0f, 0.0f, 0.0f);

    if (glm::l2Norm(direction) > 0) {
        const float speed = .5f;
        player->move(speed * glm::normalize(direction));
        camera->move(player->getWPosition());
    }

    return false;
}
