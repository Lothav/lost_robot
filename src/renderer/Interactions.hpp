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
#include <functional>

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
            player_ = new Renderer::Player(glm::vec3(0.f , 0.f, 0.8f));
            player_->importFromFile("./data/players/", "untitled.fbx", {GL_RGB});
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

            auto position = player_->getPosition();

            glm::vec3 direction(0.0f);

            float beta = std::uniform_real_distribution<float>(-1.0f, 1.0f)(generator);
            direction += beta * glm::vec3(1.0f, 0.0, 0.f);

            beta = std::uniform_real_distribution<float>(-1.0f, 1.0f)(generator);
            direction += beta * glm::vec3(0.0f, 1.0, 0.f);

            float distance = std::uniform_real_distribution<float>(stddev, 3 *  stddev)(generator);
            direction = distance * glm::normalize(direction);
            position += direction;

            auto npc = new NPC(*spider_npc_model);  // copy of model
            npc->setPosition(glm::vec3(position.x, position.y, 0.5f));

            npcs_.push_back(npc);
            Renderer::BulkObject3D::getInstance().push_back(npc);
        }

        void fire(Renderer::Player *player)
        {
            const int max_projectiles = 3;
            if (this->projectiles_.size() > max_projectiles) return;

            auto projectile = new Renderer::Object3D(*projectile_model);
            projectile->transformModel(
                glm::translate(glm::scale(player->getModelMatrix(), glm::vec3(.5)), glm::vec3(0.0, 0.0f, 2.0f))
            );

            projectiles_.push_back(projectile);
            Renderer::BulkObject3D::getInstance().push_back(projectile);
        }

        void timeTick(
            const std::function<void()> &reset_spawn_cycle
        )
        {
            const glm::vec3 &player_pos = player_->getWPosition();

            std::vector<NPC *> npc_swp;
            std::vector<Object3D *> projectile_swp;

            for (size_t i = 0; projectiles_.size() > i; i++) {
                const auto &projectile_ = projectiles_[i];

                projectile_->move(glm::vec3(0.0, 3.0f, 0.f));

                glm::vec3 object_position = projectile_->getWPosition();
                AxisAlignedBB object_bb = projectile_->getAABB();

                bool collision = false;

                npc_swp.clear();

                for (const auto &npc : npcs_) {
                    AxisAlignedBB npc_bb = npc->getAABB();
                    if (Renderer::Collisions::check(&object_bb, &npc_bb)) {
                        collision = true;

                        if (npc->shoot()) {
                            player_->scored();
                            BulkObject3D::getInstance().remove(npc);
                            delete npc;
                        }
                        else npc_swp.push_back(npc);
                    }
                    else {
                        npc_swp.push_back(npc);
                    }
                }
                npcs_ = npc_swp;


                const float threshold = 4.0f;
                if (!collision && glm::distance2(object_position, player_pos) < threshold) projectile_swp.push_back(projectile_);
                else {
                    BulkObject3D::getInstance().remove(projectile_);
                    delete projectile_;
                }
            }

            projectiles_ = projectile_swp;

            const float inf = 1.0f / 1e-12f;

            bool hurt = false;

            for (auto &npc : npcs_) {

                auto npc_z = this->ground_->getZbyXY(npc->getPosition(), 1 / PLAYER_SCALE);
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
                    } else {
                        if (player_->hurt()) {
                            player_->resetLives();
                            player_->move(-player_->getPosition());
                            auto player_z = ground_->getZbyXY(player_->getPosition(), 1 / PLAYER_SCALE);
                            if (player_z > 0) player_->updateZ(player_z);
                            reset_spawn_cycle();
                        }

                        hurt = true;
                    }

                }

                delta = player_pos - npc_position;
                GLfloat angle = glm::degrees(glm::angle(glm::normalize(delta), glm::vec3(0.0f, 1.0f, 0.0f)));
                if (delta.x > 0) angle *= -1;
                npc->turn(angle);
            }

            if (hurt) {
                for (auto &npc : npcs_) {
                    BulkObject3D::getInstance().remove(npc);
                    delete npc;
                }
                npcs_.clear();
            }
        }
    };
}

#endif //MOONRAT_PROJECTILE_HPP
