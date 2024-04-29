//
// Created by HIMA on 4/29/2024.
//
#include "collision.hpp"
#include "components/Paimon.hpp"

namespace our {

    void CollisionSystem::init(Application *app) {
        this->app = app;
        counter = 0;
    }

    int CollisionSystem::update(World *world) {
        int count = 0;
        for (auto entity: world->getEntities()) {
            if (entity->getComponent<Paimon>() != nullptr) {
                paimonPos = entity->getWorldPosition();
                paimon = entity;

                break;
            }
        }
        for (auto entity: world->getEntities()) {
            glm::vec3 moraVec = entity->getWorldPosition();
            Mora *moraObject = entity->getComponent<Mora>();

            if (moraObject != nullptr) {
                auto len = glm::length(paimonPos - moraVec);
                //std::cout << "Len: " << len << std::endl;

                if (len <  1.1f) {
                    //moraObject->getOwner()->localTransform.position[1] = 100;
                    //std::cout << "Mora Hit" << std::endl;
                    world->markForRemoval(entity);
                    count ++;
                }
            }
        }

        return count;
    }

    void CollisionSystem::checkGameOver(bool gameOverflag) {
        // Implement game over logic here
    }

    void CollisionSystem::exit() {
        app = nullptr;
        mora = nullptr;
        paimon = nullptr;
    }
}