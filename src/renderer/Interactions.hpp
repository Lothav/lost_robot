//
// Created by ivan on 16/06/18.
//

#ifndef MOONRAT_PROJECTILE_HPP
#define MOONRAT_PROJECTILE_HPP


#include "Player.hpp"
#include "BulkObject3D.hpp"
#include "Collisions.hpp"
#include "NPC.hpp"
#include "Ground.hpp"

#include <glm/gtx/norm.hpp>
#include <random>

namespace Renderer
{
    class Interactions {

    private:
        std::default_random_engine generator;

        Player *player_;
        std::vector<NPC *> npcs_;
        std::vector<Object3D *> projectiles_;

        Ground*     ground_;
        NPC*        spider_npc_model;
        Object3D*   projectile_model;

    public:

        static Interactions& getInstance();

        void setupGround(Ground* ground) {
            this->ground_ = ground;
        }

        Player *setupPlayer()
        {
            player_ = new Renderer::Player(glm::vec3(0.f));
            player_->importFromFile("./data/players/", "robot_1.fbx", {GL_RGB});
            player_->transformModel(glm::scale(glm::mat4(1.0f), glm::vec3(PLAYER_SCALE)));
            player_->transformVertices(glm::rotate(glm::radians(90.f), glm::vec3(1.f, .0f, .0f)));
            player_->transformVertices(glm::rotate(glm::radians(180.f), glm::vec3(.0f, .0f, 1.f)));
            Renderer::BulkObject3D::getInstance().push_back(player_);
            return player_;
        }

        void loadDefaultModels()
        {
            spider_npc_model = new NPC(glm::vec3());
            spider_npc_model->importFromFile("./data/mobs/spider/", "with_texture.dae", {GL_RGBA, GL_RGB});
            // spider_npc_model->importFromFile("./data/mobs/wolf/", "testao.fbx", {GL_RGB, GL_RGB, GL_RGB, GL_RGB, GL_RGB});
            spider_npc_model->transformModel(glm::scale(glm::mat4(1.0f), glm::vec3(PLAYER_SCALE)));

            projectile_model = new Renderer::Object3D(glm::vec3(0.f));
            projectile_model->importFromFile("./data/environment/metal_water_tank/", "Water_Tank_fbx.fbx", {GL_RGB});
        }

        void spawnNPC()
        {
            const float stddev = 0.5f / PLAYER_SCALE;

            auto player_position = player_->getPosition();

            float x = std::normal_distribution<float>(player_position.x, stddev)(generator);
            float y = std::normal_distribution<float>(player_position.y, stddev)(generator);

            auto npc = new NPC(*spider_npc_model); // copy of model
            npc->setPosition(glm::vec3(x, y, 0.5f));

            npcs_.push_back(npc);
            Renderer::BulkObject3D::getInstance().push_back(npc);
        }

        void fire(Renderer::Player *player)
        {
            auto projectile = new Renderer::Object3D(*projectile_model);
            projectile->transformModel(
                glm::translate(glm::scale(player->getModelMatrix(), glm::vec3(.5)), glm::vec3(0.0, 0.0f, 3.0f))
            );

            projectiles_.push_back(projectile);
            Renderer::BulkObject3D::getInstance().push_back(projectile);
        }

        void timeTick()
        {
            const glm::vec3 &player_pos = player_->getWPosition();

            std::vector<Object3D *> swp;

            for (size_t i = 0; projectiles_.size() > i; i++) {
                const auto &o = projectiles_[i];

                o->move(glm::vec3(0.0, 3.0f, 0.f));

                glm::vec3 object_position = o->getWPosition();
                AxisAlignedBB object_bb = o->getAABB();

                bool collision = false;

                for (const auto &npc : npcs_) {
                    AxisAlignedBB npc_bb = npc->getAABB();
                    if (Renderer::Collisions::check(&object_bb, &npc_bb)) collision = true;
                }

                const float threshold = 4.0f;
                if (!collision && glm::distance2(object_position, player_pos) < threshold) swp.push_back(o);
                else {
                    BulkObject3D::getInstance().remove(o);
                }

            }

            projectiles_ = swp;

            const float inf = 1.0f / 1e-12f;

            for (auto &npc : npcs_) {

                auto npc_z = this->ground_->getZbyXY(npc->getPosition(), 1/PLAYER_SCALE);
                if (npc_z > 0) {
                    npc->updateZ(npc_z);
                }

                const float speed = .25f;
                const float threshold = .25f;

                auto npc_position = npc->getWPosition();

                float closest_dist = inf;
                glm::vec3 closest(0.0f);

                for (auto &other : npcs_) {
                    if (other == npc) continue;

                    const glm::vec3 &other_position = other->getWPosition();
                    auto current_dist = glm::distance2(npc_position, other_position);

                    if (current_dist < closest_dist) {
                        closest = other_position;
                        closest_dist = current_dist;
                    }
                }

                glm::vec3 delta;

                if (closest_dist < threshold) {
                    const float alpha = 0.5f;
                    delta = alpha * (npc_position - closest) + (1 - alpha) * (player_pos - npc_position);
                    npc->move(speed * glm::normalize(delta));
                }
                else {
                    delta = player_pos - npc_position;

                    if (glm::l2Norm(delta) > threshold) {
                        npc->move(speed * glm::normalize(delta));
                    }

                }

                delta = player_pos - npc_position;
                GLfloat angle = glm::degrees(glm::angle(glm::normalize(delta), glm::vec3(0.0f, 1.0f, 0.0f)));
                if (delta.x > 0) angle *= -1;
                npc->turn(angle);
            }
        }
    };
}

#endif //MOONRAT_PROJECTILE_HPP
