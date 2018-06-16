//
// Created by luiz0tavio on 6/4/18.
//

#ifndef MOONRAT_INPUT_HPP
#define MOONRAT_INPUT_HPP

#include <SDL2/SDL_events.h>
#include "../renderer/Camera.hpp"
#include "../memory/Allocator.hpp"

namespace Events
{
    class Input
    {
    public:

        static Input& getInstance();

        Input(Input const&) = delete;
        void operator=(Input const&)  = delete;

    private:

        Input() {}

        std::map<Uint32, bool> key_pressed_;

    public:

        bool HandleEvent(Renderer::Camera* camera, std::vector<Renderer::Object3D *, Memory::Allocator<Renderer::Object3D *>> objects)
        {
            SDL_Event e;
            while (SDL_PollEvent(&e)) {

                if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_q)) {
                    return true;
                }

                int mouse_pos_x, mouse_pos_y;
                SDL_GetMouseState(&mouse_pos_x, &mouse_pos_y);


                // Restart if hit 'r'
                if (e.type == SDL_KEYDOWN) {

                    switch (e.key.keysym.sym) {

                        case SDLK_w:
                            //player->move(.0f, .01f);
                            camera->move({.0f, -.01f, .0f});
                            break;

                        case SDLK_a:
                            //player->move(-.01f, .0f);
                            camera->move({.01f, .0f, .0f});
                            break;

                        case SDLK_s:
                            //player->move(.0f, -.01f);
                            camera->move({.0f, .01f, .0f});
                            break;

                        case SDLK_d:
                            //player->move(.01f, .0f);
                            camera->move({-.01f, .0f, .0f});
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

                if (e.type == SDL_MOUSEBUTTONDOWN) {
                    std::cout << "false" << std::endl;
                    this->key_pressed_[e.button.button] = true;
                }

                if(e.type == SDL_MOUSEBUTTONUP) {
                    std::cout << "true" << std::endl;
                    this->key_pressed_[e.button.button] = false;
                }

                if(e.type == SDL_MOUSEMOTION) {
                    std::cout << "motion" << std::endl;
                    if (this->key_pressed_[SDL_BUTTON_RIGHT]) {
                        for (auto object: objects) {
                            object->rotate(
                                static_cast<GLfloat>(mouse_pos_x) / camera->getWindowSize()[0],
                                static_cast<GLfloat>(mouse_pos_y) / camera->getWindowSize()[1]);
                        }
                    }

                }

            }
            return false;
        }

    };
}

#endif //MOONRAT_INPUT_HPP
