#include <vector>
#include <stack>

#include "Algorithms/l_system.h"
#include "utils.h"

#include "Render/render_system.h"
#include "Systems/ecs_manager.h"

#include "Components\components.h"

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>
#include <../deps/glm/gtc/quaternion.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <../deps/glm/gtx/matrix_decompose.hpp>

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
    std::stack<glm::vec3> scale_stack;

    size_t parent_entity = 0;

    for (char c : final_string) {
        switch (c) {
        case 'F': {
            TransformComponent tc;
            if (parent_entity != 0) {
                glm::mat4 parentWorld = rs_.getWorldTransform(parent_entity);
                glm::mat4 invParent = glm::inverse(parentWorld);

                glm::mat4 world_transform =
                    glm::translate(glm::mat4(1.0f), position_) *
                    glm::mat4_cast(rotation_) *
                    glm::scale(glm::mat4(1.0f), cube_size_);

                glm::mat4 local_transform = invParent * world_transform;

                glm::vec3 skew;
                glm::vec4 perspective;
                glm::decompose(local_transform, tc.scale, tc.rotation, tc.position, skew, perspective);
            }
            else {
                tc.position = position_;
                tc.rotation = rotation_;
                tc.scale = cube_size_;
            }

            
            size_t entity_id = rs_.createCube8v(tc, 1.0f, false);

            entity_id--; 
           
            if (parent_entity == 0) {
                parent_entity = entity_id;
            }
            else {
                if (ecs_.has_component<HierarchyComponent>(parent_entity) && ecs_.has_component<HierarchyComponent>(entity_id)) {
                    auto& childHierarchy = ecs_.get_component<HierarchyComponent>(entity_id);
                    childHierarchy->addParent((int)parent_entity);

                    auto& parentHierarchy = ecs_.get_component<HierarchyComponent>(parent_entity);
                    parentHierarchy->addChild((int)entity_id);
                }
            }

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
            scale_stack.push(cube_size_);
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
            if (!scale_stack.empty()) {
                cube_size_ = scale_stack.top();
                scale_stack.pop();
            }
            break;
        }
        }
    }

    if (parent_entity != 0) {
        HierarchyTree(parent_entity);
    }

    rs_.initRenderTypes();
}

void LSystem::HierarchyTree(size_t entity_id) {
    if (ecs_.has_component<HierarchyComponent>(entity_id)) {
        auto& hierarchy = ecs_.get_component<HierarchyComponent>(entity_id);

        for (auto childId : hierarchy->getChildren()) {
            HierarchyTree(childId);
        }
    }
}

