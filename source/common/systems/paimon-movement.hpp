//
// Created by xAbdoMo on 4/23/2024.
//

#ifndef GFX_LAB_PAIMON_MOVEMENT_HPP
#define GFX_LAB_PAIMON_MOVEMENT_HPP

#include "Level-mapping.hpp"

#define BLOCK_REACH_MAX_DIFF 0.1

namespace our {
    class PaimonMovement {
    private:
        Application* app{};
        Ground* currentTarget;
        Ground* nextBlock;
        glm::vec3 nextBlockPosition;

    public:
        void init(Application* a){
            app = a;
        }

        void update(World *world, LevelMapping* level, float deltaTime){

            //first we get paimon
            Paimon* paimon = nullptr;
            CameraComponent* camera = nullptr;

            for (auto k : world->getEntities()){
                if (paimon == nullptr) paimon = k->getComponent<Paimon>();
                if (camera == nullptr) camera = k->getComponent<CameraComponent>();
            }

            if (!camera || !paimon) return;

            if (currentTarget && nextBlock){
                auto dis = level->getPaimonDistanceToGround(nextBlockPosition);
                if (dis <= BLOCK_REACH_MAX_DIFF){
                    //block reached
                    auto route = level->findRoute(currentTarget);
                    std::cout << "Reached end: " << route.size() << std::endl;
                    if (route.size() > 1){
                        nextBlock = route[1].ground;
                        nextBlockPosition = route[1].fakePosition;
                    } else {
                        currentTarget = nullptr;
                        nextBlock = nullptr;
                    }
                }else{
//                    auto nextBlockRoute = level->findRoute(nextBlock);
//                    if (nextBlockRoute.empty()){ //can't even go to the next block ..
//                        currentTarget = nullptr;
//                        nextBlock = nullptr;
//                        return;
//                    }

                    glm::vec3 paimonUp       = glm::vec3(
                            camera->getViewMatrix() * paimon->getOwner()->getLocalToWorldMatrix() *
                            glm::vec4(0 , 1 , 0 , 0.0)
                    );

                    paimonUp = glm::normalize(paimonUp);

                    auto pos1 = paimon->getOwner()->localTransform.position;
                    auto pos2 = glm::vec3(glm::inverse(camera->getViewMatrix()) * glm::vec4(nextBlockPosition + paimonUp * PAIMON_TO_BLOCK_OFFSET , 1.0));

                    paimon->getOwner()->localTransform.position += glm::normalize(pos2 - pos1) * paimon->speed * deltaTime;
                }

                return;
            }

            auto target = level->ScreenToGroundCast(app->getMouse().getMousePosition().x , app->getMouse().getMousePosition().y);

            if (app->getMouse().isPressed(0)){ //left click
                auto route = level->findRoute(target);
                if (route.size() > 1){
                    currentTarget = target;
                    nextBlock = route[1].ground;
                    nextBlockPosition = route[1].fakePosition;
                }
            }
        }
    };
}

#endif //GFX_LAB_PAIMON_MOVEMENT_HPP
