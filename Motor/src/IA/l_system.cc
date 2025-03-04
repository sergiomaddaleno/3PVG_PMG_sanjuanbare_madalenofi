#include <vector>
#include <stack>

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

LSystem::LSystem() : position(0.0f, 0.0f, 0.0f), rotation(1.0f, 0.0f, 0.0f, 0.0f)
{
}

LSystem::~LSystem()
{
}

void LSystem::init(std::string axiom, std::map<char, std::string> rule_map, int iterations) {
    std::string final_string = axiom;

    for (int i = 0; i < iterations; ++i) {
        std::string new_axiom = "";
        for (char c : final_string) {
            if (rule_map.find(c) != rule_map.end()) {
                new_axiom += rule_map[c];
            }
            else {
                new_axiom += c;
            }
        }
        final_string = new_axiom;
    }

    std::stack<glm::vec3> position_stack;
    std::stack<glm::quat> rotation_stack;

    for (char c : final_string) {
        switch (c) {
        case 'F': {
            TransformComponent tc;
            tc.position = position;
            tc.rotation = rotation;
            tc.scale = glm::vec3(cube_size);
            global.render_system.createCube8v(global.ecs, tc);

            glm::vec3 forward = rotation * glm::vec3(0.0f, 1.0f, 0.0f);
            position += forward * (cube_size * spacing);
            break;
        }
        case 'f': {
            glm::vec3 forward = rotation * glm::vec3(1.0f, 0.0f, 0.0f);
            position += forward * (cube_size * spacing);
            break;
        }
        case '+': {
            rotation = glm::rotate(rotation, angle, glm::vec3(1.0f, 0.0f, 0.0f));
            break;
        }
        case '-': {
            rotation = glm::rotate(rotation, -angle, glm::vec3(1.0f, 0.0f, 0.0f));
            break;
        }
        case '[': {
            position_stack.push(position);
            rotation_stack.push(rotation);
            break;
        }
        case ']': {
            if (!position_stack.empty()) {
                position = position_stack.top();
                position_stack.pop();
            }
            if (!rotation_stack.empty()) {
                rotation = rotation_stack.top();
                rotation_stack.pop();
            }
            break;
        }
        }
    }

    global.render_system.init(global.ecs);
}


