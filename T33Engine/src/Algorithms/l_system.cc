#include <vector>
#include <stack>

#include "Algorithms/l_system.h"
#include "utils.h"
#include "../include/global_data.h"

#include "Render/render_system.h"
#include "Systems/ecs_manager.h"

#include "Components\transform_component.h"

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>
#include <../deps/glm/gtc/quaternion.hpp>

LSystem::LSystem(ECSManager& ecs, RenderSystem& rs) :
    starting_pos_(0.0f, 0.0f, 0.0f),
    starting_rot_(1.0f, 0.0f, 0.0f, 0.0f),
    position_(starting_pos_),
    rotation_(starting_rot_),
    angle_(glm::radians(30.0f)),
    cube_size_(0.4f, 2.0f, 0.4f),
    spacing_(2.0f),
    ecs_(ecs),                    
    rs_(rs) 
{}

LSystem::~LSystem()
{
}

void LSystem::init(std::string axiom, std::map<char, std::string> rule_map, int iterations) {
    
    position_ = starting_pos_;
    rotation_ = starting_rot_;
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
            tc.position = position_;
            tc.rotation = rotation_;
            tc.scale = cube_size_;
            rs_.createCube8v(tc, 1.0f, false);

            glm::vec3 forward = rotation_ * glm::vec3(0.0f, 1.0f, 0.0f);
            position_ += forward * (cube_size_.y * spacing_);
            break;
        }
        case 'f': {
            glm::vec3 forward = rotation_ * glm::vec3(1.0f, 0.0f, 0.0f);
            position_ += forward * (cube_size_.x * spacing_);
            break;
        }
        case '+': {
            rotation_ = glm::rotate(rotation_, angle_, glm::vec3(1.0f, 0.0f, 0.0f));
            break;
        }
        case '-': {
            rotation_ = glm::rotate(rotation_, -angle_, glm::vec3(1.0f, 0.0f, 0.0f));
            break;
        }
        case '[': {
            position_stack.push(position_);
            rotation_stack.push(rotation_);
            break;
        }
        case ']': {
            if (!position_stack.empty()) {
                position_ = position_stack.top();
                position_stack.pop();
            }
            if (!rotation_stack.empty()) {
                rotation_ = rotation_stack.top();
                rotation_stack.pop();
            }
            break;
        }
        }
    }

    rs_.init();
}


