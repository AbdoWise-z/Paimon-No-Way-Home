//
// Created by HIMA on 4/29/2024.
//
#include "collision.hpp"
#include "components/Paimon.hpp"
#include "events-system-controller.hpp"

namespace our {

    void CollisionSystem::init(Application *app) {
        this->app = app;

    }

    void CollisionSystem::update(World *world , int& goldenCount , int& blueCount , int& redCount) {
        glm::vec3 paimonPos;
        Entity* paimon;
        for (auto entity: world->getEntities()) {
            if (entity->getComponent<Paimon>() != nullptr) {
                paimonPos = entity->getWorldPosition();
                paimon = entity;

                break;
            }
        }

        if (!paimon) return ;

        for (auto entity: world->getEntities()) {
            glm::vec3 moraVec = entity->getWorldPosition();
            Mora *moraObject = entity->getComponent<Mora>();

            if (moraObject != nullptr) {
                auto len = glm::length(paimonPos - moraVec + moraObject->offset);
                if (len <  1.5f) {
                    //moraObject->getOwner()->localTransform.position[1] = 100;
                    //std::cout << "Mora Hit" << std::endl;
                    our::Events::onPaimonPickMora(entity->name);
                    world->markForRemoval(entity);
                    switch (moraObject->type) {
                        case GOLDEN:
                            goldenCount++;
                            break;
                        case BLUE:
                            blueCount++;
                            break;
                        case RED:
                            redCount++;
                            break;
                    }
                }
            }
        }
    }

    void CollisionSystem::checkGameOver(bool gameOverflag) {
        // Implement game over logic here
    }

    void CollisionSystem::exit() {
        app = nullptr;
    }
}