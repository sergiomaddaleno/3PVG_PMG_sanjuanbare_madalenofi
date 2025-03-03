#include <vector>

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

void LSystem::init(std::string axiom, std::string rules, int iterations) {
    std::vector<Branch> branchStack;
    Branch branch;
    branch.position = glm::vec3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < iterations; ++i) {
        std::string new_axiom = "";

        for (char axiom_char : axiom) {
            switch (axiom_char) {
            case 'C': {
                TransformComponent tc;
                tc.position = branch.position;
                tc.scale = glm::vec3(1.0f);
                global.render_system.createCube8v(global.ecs, tc); 

                new_axiom += rules;

                break;
            }
            case 'E': {
                TransformComponent tc;
                tc.position = branch.position;
                tc.scale = glm::vec3(1.0f); 

                new_axiom += rules;
                break;
            }
            case '+': { branch.position.y += cube_size; new_axiom += rules;break; } 
            case '-': { branch.position.y -= cube_size; new_axiom += rules;break; }
            case 'R': { branch.position.x += cube_size; new_axiom += rules;break; } 
            case 'L': { branch.position.x -= cube_size; new_axiom += rules;break; }
            }
        }
        axiom = new_axiom;
    }

    global.render_system.init(global.ecs); 
}



