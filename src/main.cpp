#include <random>
#include <functional>
#include <memory>
#include <SDL2/SDL_image.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "renderer/Window.hpp"
#include "renderer/BulkText.hpp"
#include "events/Input.hpp"
#include "renderer/Object3D.hpp"
#include "renderer/BulkObject3D.hpp"
#include "renderer/Player.hpp"
#include "renderer/NPC.hpp"
#include "renderer/Interactions.hpp"
#include "renderer/Ground.hpp"

void update() {

}

int main(int argc, char *argv[]) {
    Memory::Provider::initPools();

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Could not initialize sdl2: " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }

    if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == 0) {
        std::cerr << "Could not initialize IMG's flags" << std::endl;
        return EXIT_FAILURE;
    }

    {
        SDL_DisplayMode DM;
        SDL_GetCurrentDisplayMode(0, &DM);
        std::array<int, 2> window_default_size = {DM.w, DM.h};

        auto window = std::make_unique<Renderer::Window>("Moonrat", window_default_size);

        auto mainContext = SDL_GL_CreateContext(window->getWindow());

        GLenum err = glewInit();
        if (err != GLEW_OK) {
            std::cerr << "Glew is not ok =(" << std::endl;
            return EXIT_FAILURE;
        }

        if (!GLEW_VERSION_2_0) {
            std::cerr << "Error: your graphic card does not support OpenGL 2.0" << std::endl;
            return EXIT_FAILURE;
        }

        GLfloat white_color[4]{1.f, 1.f, 1.f, 1.f};
        auto text_lives = new Renderer::Text(-0.9f, 0.8f, 62, white_color);
        Renderer::BulkText::getInstance().push_back(text_lives);

        auto text_points = new Renderer::Text(0.6f, 0.8f, 62, white_color);
        Renderer::BulkText::getInstance().push_back(text_points);

        auto ground = new Renderer::Ground();

        auto player = Renderer::Interactions::getInstance().setupPlayer();

        auto camera = new Renderer::Camera(
            Renderer::BulkObject3D::getInstance().GetShaderProgram(),
            window_default_size,
            player->getWPosition()
        );

        glAlphaFunc(GL_GREATER, 0.5);
        glEnable(GL_ALPHA_TEST);

        glDepthFunc(GL_LESS);
        glEnable(GL_DEPTH_TEST);

        Renderer::Interactions::getInstance().loadDefaultModels();
        Renderer::Interactions::getInstance().setupGround(ground);

        auto spawn = [&]() {
            Renderer::Interactions::getInstance().spawnNPC();
        };

        auto loop = [&]() -> bool {
            auto start = SDL_GetTicks();

            // Set screen to black
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            update();

            Renderer::BulkText::getInstance().draw(camera);
            Renderer::BulkObject3D::getInstance().draw(camera);

            Renderer::Interactions::getInstance().timeTick();

            auto quit = Events::Input::getInstance().HandleEvent(camera, player, spawn);

            auto player_z = ground->getZbyXY(player->getPosition(), 1/PLAYER_SCALE);
            if (player_z > 0) {
                player->updateZ(player_z);
            }

            text_lives->setText("Lives: " + std::to_string(player->getLives()));
            text_points->setText("Points: " + player->getPointsText());

            if (quit) return false;

            // Swap Window
            SDL_GL_SwapWindow(window->getWindow());

            // Adjust FPS
            if (1000 / 60 > (SDL_GetTicks() - start)) {
                SDL_Delay(1000 / 60 - (SDL_GetTicks() - start));
            }

            return true;
        };

        while (loop());

        SDL_GL_DeleteContext(mainContext);
    }

    Memory::Provider::destroyPools();
    SDL_Quit();
    glEnd();

    return EXIT_SUCCESS;
}
