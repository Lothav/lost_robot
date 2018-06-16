//
// Created by luiz0tavio on 6/4/18.
//

#ifndef MOONRAT_INPUT_HPP
#define MOONRAT_INPUT_HPP

#include <SDL2/SDL_events.h>
#include "../renderer/Camera.hpp"
#include "../memory/Allocator.hpp"
#include "../renderer/Player.hpp"

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

        std::map<SDL_Keycode, bool> key_pressed_;

        GLfloat mouse_angle_;

    public:

        bool HandleEvent(Renderer::Camera* camera, Renderer::Player *player) ;

    };
}

#endif //MOONRAT_INPUT_HPP
