//
// Created by ivan on 16/06/18.
//

#ifndef MOONRAT_PROJECTILE_HPP
#define MOONRAT_PROJECTILE_HPP


#include "Player.hpp"
#include "BulkObject3D.hpp"
#include "Collisions.hpp"

#include <glm/gtx/norm.hpp>

namespace Renderer {
    class Projectiles {

    private:
        std::vector<Object3D *> objects_;

    public:
        static Projectiles& getInstance();


        void fire(Renderer::Player *player)
        {
            auto projectile = new Renderer::Object3D(glm::vec3(0.f));
            projectile->importFromFile("./data/environment/metal_water_tank/", "Water_Tank_fbx.fbx");

            projectile->transform(
                glm::translate(glm::scale(player->getModelMatrix(), glm::vec3(.5)), glm::vec3(0.0, 0.0f, 3.0f))
            );

            objects_.push_back(projectile);

            Renderer::BulkObject3D::getInstance().push_back(projectile);
        }

        void timeTick(Renderer::Player *player, Renderer::Object3D *npc)
        {
            const float threshold = 4.0f;

            const glm::vec3 &player_pos = player->getPosition();

            AxisAlignedBB npc_bb = npc->getAABB();

            std::vector<Object3D *> swp;

            for (size_t i = 0; objects_.size() > i; i++) {
                const auto &o = objects_[i];

                o->move(glm::vec3(0.0, 3.0f, 0.f));

                glm::vec3 object_position = o->getPosition();

                AxisAlignedBB object_bb = o->getAABB();

                if (!Renderer::Collisions::check(&object_bb, &npc_bb) && glm::distance2(object_position, player_pos) < threshold) swp.push_back(o);
                else BulkObject3D::getInstance().remove(o);
            }

            objects_ = swp;
        }
    };
}

#endif //MOONRAT_PROJECTILE_HPP
