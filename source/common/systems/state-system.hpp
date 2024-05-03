//
// Created by xAbdoMo on 4/27/2024.
//

#ifndef GFX_LAB_STATE_SYSTEM_HPP
#define GFX_LAB_STATE_SYSTEM_HPP

#include "ecs/world.hpp"
#include "components/actions/StateAnimator.h"
#include "components/mesh-renderer.hpp"
#include "ground-system.hpp"

namespace our{
    class StateSystem {
    private:
        template<typename T>
        inline T lerp(const T& a, const T& b, float val){
            return a * (1 - val) + val * b;
        }
        inline void update_state(StateAnimator* state, float deltaTime , World* world){
            if (state->currentState != state->nextState){
                state->transitionProgress += deltaTime;

                bool done = false;
                if (state->transitionProgress > state->transitionDuration){
                    done = true;
                    state->transitionProgress = state->transitionDuration;
                }
                float val = state->transitionProgress / state->transitionDuration;
                //TODO maybe add another lerp functions ?

                auto pos = lerp(
                        state->states[state->currentState].position ,
                        state->states[state->nextState].position ,
                        val
                );

                auto rot = lerp(
                        state->states[state->currentState].rotation ,
                        state->states[state->nextState].rotation ,
                        val
                );

                auto scl = lerp(
                        state->states[state->currentState].scale ,
                        state->states[state->nextState].scale ,
                        val
                );

                auto tnt = lerp(
                        state->states[state->currentState].tint ,
                        state->states[state->nextState].tint ,
                        val
                );


                auto k = state->getOwner();

                std::vector<Entity*> children;
                std::vector<glm::vec3> positions;
                for (auto child : world->getEntities()){
                    if (child->getComponent<Ground>() && child->hasAncestor(k)){
                        positions.push_back(child->getWorldPosition());
                        children.push_back(child);
                    }
                }

                if (state->position) {
                    auto diff = k->getWorldPosition();
                    k->localTransform.position = pos;
                    diff = k->getWorldPosition() - diff;
                    auto g = k->getComponent<Ground>();
                    if (g) our::GroundSystem::onGroundMoved(g , diff);
                }

                if (state->scale)    k->localTransform.scale    = scl;
                if (state->rotation) k->localTransform.rotation = rot;
                if (state->tint) {
                    for (auto renderer: k->getAllComponents<MeshRendererComponent>()) {
                        auto mat = (DefaultMaterial *) renderer->material;
                        mat->tint = tnt;
                    }
                }

                for (int i = 0;i < children.size();i++){
                    auto diff = children[i]->getWorldPosition() - positions[i];
                    our::GroundSystem::onGroundMoved(children[i]->getComponent<Ground>() , diff);
                }

                if (done){
                    state->currentState = state->nextState;
                    k->enabled = state->states[state->currentState].enabled;
                }
            }
        }
    public:
        void init(World* world){
            for (auto k : world->getEntities()){
                auto state = k->getComponent<StateAnimator>();
                if (state){
                    state->nextState = state->currentState;
                    if (state->position) k->localTransform.position = state->states[state->currentState].position;
                    if (state->scale   ) k->localTransform.scale    = state->states[state->currentState].scale;
                    if (state->rotation) k->localTransform.rotation = state->states[state->currentState].rotation;
                    k->enabled = state->states[state->currentState].enabled;

                    if (state->tint){
                        for (auto renderer: k->getAllComponents<MeshRendererComponent>()) {
                            auto mat = (DefaultMaterial *) renderer->material;
                            mat->tint = state->states[state->currentState].tint;
                        }
                    }
                }
            }
        }

        void update(World* world, float deltaTime){
            for (auto k : world->getEntities()){
                auto state = k->getComponent<StateAnimator>();
                if (state && state->currentState != state->nextState){
                    update_state(state , deltaTime , world);
                }
            }
        }
    };
}

#endif //GFX_LAB_STATE_SYSTEM_HPP
