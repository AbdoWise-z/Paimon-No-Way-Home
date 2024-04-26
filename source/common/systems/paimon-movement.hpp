//
// Created by xAbdoMo on 4/23/2024.
//

#ifndef GFX_LAB_PAIMON_MOVEMENT_HPP
#define GFX_LAB_PAIMON_MOVEMENT_HPP

#include "Level-mapping.hpp"

#define BLOCK_REACH_MAX_DIFF 0.05

namespace our {
    class PaimonMovement {
    private:
        Application* app{};
        Ground* currentTarget;
        Ground* nextBlock;
        glm::vec3 nextBlockPosition;
        bool returnToBlockCenter = false;

        inline void update_angle(Paimon* paimon, CameraComponent* camera, glm::vec3 diff , float deltaTime){
            auto diff3D = glm::inverse(camera->getViewMatrix()) * glm::vec4(diff, 0.0);
            std::cout << "X: " << diff3D.x << " , Y: " << diff3D.y << " , Z: " << diff3D.z << std::endl;
            diff3D.y = 0;
            diff3D = glm::normalize(diff3D);
            auto dx = abs(diff3D.x);
            auto dz = abs(diff3D.z);

            auto angle = 0.0f;
            if (dx > dz){
                if (diff3D.x > 0){
                    angle = 0;
                }else {
                    angle = 180;
                }
            }else{
                if (diff3D.z > 0){
                    angle = -90;
                }else {
                    angle = 90;
                }
            }

            if (
                    abs(glm::radians(angle) - paimon->getOwner()->localTransform.rotation.y) <
                    abs(glm::radians(angle) + glm::two_pi<float>() - paimon->getOwner()->localTransform.rotation.y) ){
                angle = glm::radians(angle) - paimon->getOwner()->localTransform.rotation.y;
            }else{
                angle = glm::radians(angle) + glm::two_pi<float>() - paimon->getOwner()->localTransform.rotation.y;
            }

            if (angle != 0)
                paimon->getOwner()->localTransform.rotation.y += angle / glm::abs(angle) * glm::radians(paimon->rotateSpeed) * deltaTime;
        }

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
            level->update();

            glm::vec3 paimonUp       = glm::vec3(
                    camera->getViewMatrix() * paimon->getOwner()->getLocalToWorldMatrix() *
                    glm::vec4(0 , 1 , 0 , 0.0)
            );
            paimonUp = glm::normalize(paimonUp);

            auto target = level->ScreenToGroundCast(app->getMouse().getMousePosition().x , app->getMouse().getMousePosition().y);

            if (returnToBlockCenter){
                auto pos1 = glm::vec3(camera->getViewMatrix() * glm::vec4(paimon->getOwner()->localTransform.position , 1.0));
                auto pos2 = nextBlockPosition + paimonUp * PAIMON_TO_BLOCK_OFFSET;
                auto diff = pos2 - pos1;
                diff.z = 0;
                pos1.z = glm::max(pos1.z , pos2.z + PAIMON_TO_BLOCK_OFFSET);
                pos1 += glm::normalize(diff) * paimon->speed * deltaTime;
                paimon->getOwner()->localTransform.position = glm::vec3(glm::inverse(camera->getViewMatrix()) * glm::vec4(pos1 , 1.0));
                update_angle(paimon, camera, diff , deltaTime);
                auto dis = level->getPaimonDistanceToGround2D(nextBlockPosition);
                if (dis <= BLOCK_REACH_MAX_DIFF){
                    returnToBlockCenter = false;
                    paimon->getOwner()->localTransform.position = glm::vec3(glm::inverse(camera->getViewMatrix()) * glm::vec4(pos2 , 1.0));
                }
            }

            if (currentTarget && nextBlock){
                auto dis = level->getPaimonDistanceToGround2D(nextBlockPosition);
                if (dis <= BLOCK_REACH_MAX_DIFF){
                    //block reached

                    auto route = level->findRoute(currentTarget);
                    if (route.size() > 1){
                        nextBlock = route[1].ground;
                        nextBlockPosition = route[1].fakePosition;
                        std::cout << "Next block end: " << route[1].blockIndex << std::endl;
                    } else {
                        currentTarget = nullptr;
                        nextBlock = nullptr;
                    }
                }else{

                    auto myBlock = level->getBlockPosition(paimon->ground);
                    auto nextBlockRoute = level->findRoute(currentTarget);
                    if (nextBlockRoute.empty()){
                        currentTarget = nullptr;
                        nextBlock = nullptr;
                        returnToBlockCenter = true;
                        nextBlockPosition = myBlock;
                        std::cout << "Return to center" << std::endl;
                        return;
                    }

                    float distance = level->getPaimonDistanceToGround(myBlock);
                    if (distance >= BLOCK_WIDTH){
                        paimon->ground = nextBlock;
                        level->update();
                    }


                    auto pos1 = glm::vec3(camera->getViewMatrix() * glm::vec4(paimon->getOwner()->localTransform.position , 1.0));
                    auto pos2 = nextBlockPosition + paimonUp * PAIMON_TO_BLOCK_OFFSET;
                    auto diff = pos2 - pos1;
                    diff.z = 0;
                    pos1.z = glm::max(myBlock.z , pos2.z) + PAIMON_TO_BLOCK_OFFSET;
                    diff   = glm::normalize(diff);
                    pos1 += diff * paimon->speed * deltaTime;
                    paimon->getOwner()->localTransform.position = glm::vec3(glm::inverse(camera->getViewMatrix()) * glm::vec4(pos1 , 1.0));

                    update_angle(paimon, camera, diff , deltaTime);
                }

                return;
            }


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
