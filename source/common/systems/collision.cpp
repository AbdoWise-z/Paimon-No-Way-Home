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

    void CollisionSystem::update(World *world, ForwardRenderer *forwardRenderer, bool &gameOverflag, AudioPlayer *audioPlayer) {
        for (auto entity: world->getEntities()) {
            if (entity->getComponent<Paimon>() != nullptr) {
                paimonPos = entity->getWorldPosition();
                paimon = entity;

                break;
            }
        }
        Mora *moraObject = nullptr;
        for (auto entity: world->getEntities()) {
            std::string name = entity->name;
            glm::vec3 moraVec = entity->getWorldPosition();
            moraObject = entity->getComponent<Mora>();

            if (moraObject != nullptr) {

                if (glm::length(paimonPos - moraVec) <  1  ) {
                    moraObject->getOwner()->localTransform.position[1] = 100;
                    id++;
                    //TODO: delete mora
                }
            }
        }
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