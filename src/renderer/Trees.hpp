//
// Created by luiz0tavio on 6/17/18.
//

#ifndef MOONRAT_TREES_HPP
#define MOONRAT_TREES_HPP

#include <array>
#include "Object3D.hpp"
#include "BulkObject3D.hpp"
#include <glm/gtx/transform.hpp>

namespace Renderer
{
    class Trees
    {

    private:

        typedef struct {
            std::string source_path;
            std::string file_name;
            std::vector<GLenum> formats;
        } Tree;

        std::vector<Tree*> trees_;

    public:

        Trees ()
        {
            auto tree01 = new Tree();
            tree01->source_path = "./data/environment/tree01/";
            tree01->file_name = "Tree.fbx";
            tree01->formats = {GL_RGB, GL_RGBA};

            trees_.push_back(tree01);

            std::array<glm::vec3, 3> trees_pos = {
                    glm::vec3(0.f, 0.2f, 0.f),
                    glm::vec3(1.f, -0.2f, 0.f),
                    glm::vec3(-1.0f, -1.0f, 0.f),
            };

            for (auto tree_pos: trees_pos)
            {
                auto tree = new Renderer::Object3D(tree_pos);
                tree->importFromFile(tree01->source_path, tree01->file_name, tree01->formats);
                tree->transformVertices(glm::scale(glm::mat4(1.0f), glm::vec3(0.5f)));
                tree->transformVertices(glm::rotate(glm::radians(90.f), glm::vec3(1.0f, 0.0f, .0f)));
                Renderer::BulkObject3D::getInstance().push_back(tree);
            }

        }
    };
}



#endif //MOONRAT_TREES_HPP
