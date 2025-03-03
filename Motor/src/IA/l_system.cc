
#include "IA/l_system.h"
#include "utils.h"
#include "../include/global_data.h"

#include "Render/render_system.h"
#include "Systems/ecs_manager.h"

#include "Components\transform_component.h"


#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>
#include <../deps/glm/gtc/quaternion.hpp>

LSystem::LSystem()
{
}

LSystem::~LSystem()
{
}

void LSystem::init(std::string& axiom, std::string& rule, int iterations)
{
    axiom_ = axiom;
    rule_ = rule;

    Branch branch;
    branch.position = glm::vec3(0.0f, 0.0f, 0.0f); 

    for (int i = 0; i < iterations; i++) {
        std::string new_axiom = "";

        for (char axiom_char : axiom_) {
            switch (axiom_char) {
            case 'F': {
                TransformComponent tc;
                tc.position = branch.position;
                tc.scale = glm::vec3(1.0f);

                global.render_system.createCube8v(global.ecs, tc);
                new_axiom += rule_;
                break;
            }
            case '+': branch.position.x += cube_size; break;
            case '-': branch.position.x -= cube_size; break;
            case '^': branch.position.y += cube_size; break;
            case 'v': branch.position.y -= cube_size; break;

            //case '[': positions.push_back(branch.position); break;
            //case ']':
            //    if (!positions.empty()) {
            //        branch.position = positions.back();
            //        positions.pop_back();
            //    }
            //    break;

            default: new_axiom += axiom_char; break;
            }
        }

        axiom_ = new_axiom;

        global.render_system.init(global.ecs);
    }
}



