//
// Created by luiz0tavio on 6/17/18.
//

#ifndef MOONRAT_GROUND_HPP
#define MOONRAT_GROUND_HPP

#include "Object3D.hpp"
#include "BulkObject3D.hpp"
#include "Trees.hpp"

namespace Renderer
{
    class Ground : public Object3D
    {

    private:

        Trees * trees_;

    public:

        Ground() : Object3D(glm::vec3(0.f))
        {
            this->importFromFile("./data/environment/terrain/", "terrain.dae", {GL_RGB, GL_RGB, GL_RGB, GL_RGB, GL_RGB});
            this->transformVertices(glm::scale(glm::mat4(1.0f), glm::vec3(10.f)));
            Renderer::BulkObject3D::getInstance().push_back(this);

            trees_ = new Trees();
        }

        float getZbyXY(glm::vec3 pos, float scale)
        {
            for (int i = 0; i < this->faces_.size(); i++) {

                auto face = this->faces_[i];

                auto vertex_0 = glm::vec3(face->vertices[0][0] * scale, face->vertices[0][1] * scale, face->vertices[0][2] * scale);
                auto vertex_1 = glm::vec3(face->vertices[1][0] * scale, face->vertices[1][1] * scale, face->vertices[1][2] * scale);
                auto vertex_2 = glm::vec3(face->vertices[2][0] * scale, face->vertices[2][1] * scale, face->vertices[2][2] * scale);

                auto b1 = sign(pos, vertex_0, vertex_1) <= 0.0f;
                auto b2 = sign(pos, vertex_1, vertex_2) <= 0.0f;
                auto b3 = sign(pos, vertex_2, vertex_0) <= 0.0f;

                if ((b1 == b2) && (b2 == b3)) {

                    float distance_0 = 1.0f/glm::pow(glm::distance(vertex_0, pos), 4);
                    float distance_1 = 1.0f/glm::pow(glm::distance(vertex_1, pos), 4);
                    float distance_2 = 1.0f/glm::pow(glm::distance(vertex_2, pos), 4);

                    return (vertex_0[2] * distance_0 + vertex_1[2] * distance_1 + vertex_2[2] * distance_2) / (distance_0 + distance_1 + distance_2);
                }
            }
            return -1;
        }
    };
}



#endif //MOONRAT_GROUND_HPP
