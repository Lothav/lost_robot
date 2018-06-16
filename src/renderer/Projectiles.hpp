//
// Created by ivan on 16/06/18.
//

#ifndef MOONRAT_PROJECTILE_HPP
#define MOONRAT_PROJECTILE_HPP


#include "Player.hpp"
#include "BulkObject3D.hpp"

#include <glm/gtx/norm.hpp>

namespace Renderer {
    class Projectiles {
    private:
        std::vector<Object3D *> objects_;

    public:
        static Projectiles& getInstance();

        void fire(Renderer::Player *player)
        {

            auto projectile = new Renderer::Object3D();
            projectile->loadTexture("./data/environment/projectile.jpg", GL_RGB);

            auto mesh = new Mesh();
            mesh->texture_index = 0;

            mesh->vertices.push_back({1.0f , 1.0f , 1.0f, 1.0f, 0.0f});
            mesh->vertices.push_back({-1.0f , -1.0f , 1.0f, 0.0f, 1.0f});
            mesh->vertices.push_back({-1.0f , 1.0f , 1.0f, 0.0f, 0.0f});
            mesh->vertices.push_back({1.0f , 1.0f , 1.0f, 1.0f, 0.0f});
            mesh->vertices.push_back({-1.0f , -1.0f , 1.0f, 0.0f, 1.0f});
            mesh->vertices.push_back({1.0f , -1.0f , 1.0f, 1.0f, 1.0f});

            projectile->addMesh(mesh);


            const auto &p = player->getPosition();

            projectile->transform(player->getModelMatrix());

            objects_.push_back(projectile);

            Renderer::BulkObject3D::getInstance().push_back(projectile);
        }

        void timeTick() {
            const float threshold = 4.0f;

            std::vector<Object3D *> swp;

            for (size_t i = 0; i < objects_.size(); i++) {
                const auto &o = objects_[i];
                o->move(glm::vec3(0.0, 1.0f, 0.f));
                if (glm::l2Norm(o->getPosition()) < threshold) swp.push_back(o);
                else BulkObject3D::getInstance().remove(o);
            }

            objects_ = swp;
        }
    };
}

#endif //MOONRAT_PROJECTILE_HPP
