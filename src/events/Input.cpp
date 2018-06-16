//
// Created by luiz0tavio on 6/4/18.
//

#include "Input.hpp"
#include "../renderer/Player.hpp"

Events::Input &Events::Input::getInstance()
{
    static Events::Input instance;
    return instance;
}

bool Events::Input::HandleEvent(Renderer::Camera *camera, Renderer::Player *player) const {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {

        if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_q)) {
            return true;
        }

        int mouse_pos_x, mouse_pos_y;
        SDL_GetMouseState(&mouse_pos_x, &mouse_pos_y);
        camera->rotate(mouse_pos_x, mouse_pos_y);

        // Restart if hit 'r'
        if (e.type == SDL_KEYDOWN) {

            switch (e.key.keysym.sym) {
                case SDLK_UP:
                    player->turn(Renderer::Player::Direction::UP);
                    break;

                case SDLK_LEFT:
                    player->turn(Renderer::Player::Direction::LEFT);
                    break;

                case SDLK_DOWN:
                    player->turn(Renderer::Player::Direction::DOWN);
                    break;

                case SDLK_RIGHT:
                    player->turn(Renderer::Player::Direction::RIGHT);
                    break;

                case SDLK_w:
                    player->move({.0f, .05f, 0.0f});
                    camera->move(player->getPosition());
                    break;

                case SDLK_a:
                    player->move({-.05f, .0f, 0.f});
                    camera->move(player->getPosition());
                    break;

                case SDLK_s:
                    player->move({.0f, -.05f, 0.0f});
                    camera->move(player->getPosition());
                    break;

                case SDLK_d:
                    player->move({.05f, .0f, 0.0f});
                    camera->move(player->getPosition());
                    break;

                case SDLK_r:
                    camera->move({.0f, .0f, .01f});
                    break;

                case SDLK_e:
                    camera->move({.0f, .0f, -.01f});
                    break;

                default:
                    break;
            }

        }

        // Restart if hit left mouse button
        if(e.type == SDL_MOUSEBUTTONDOWN) {
            if (e.button.button == SDL_BUTTON_LEFT) {
            } else if (e.button.button == SDL_BUTTON_RIGHT) {
            }
        }
    }
    return false;
}
