//
// Created by xAbdoMo on 4/28/2024.
//
#include "paimon-movement.hpp"
#include "components/OrbitalCameraComponent.h"
#include "material/material.hpp"
#include "components/mesh-renderer.hpp"
#include "ground-system.hpp"

void our::PaimonMovement::update(World *world, LevelMapping *level, float deltaTime, bool& won) {
    //first we get paimon
    for (auto k : world->getEntities()){
        if (paimon == nullptr) paimon = k->getComponent<Paimon>();
        if (camera == nullptr) camera = k->getComponent<CameraComponent>();
        if (orbitalCameraComponent == nullptr) orbitalCameraComponent = k->getComponent<OrbitalCameraComponent>();
    }


    if (!camera || !paimon || !orbitalCameraComponent) return;

    auto target = level->ScreenToGroundCast(app->getMouse().getMousePosition().x , app->getMouse().getMousePosition().y);
    if (target != nullptr){ //highlight it
        auto mat = ((DefaultMaterial*) target->getOwner()->getComponent<MeshRendererComponent>()->material);
        if (mat != lastTargetMaterial){
            if (lastTargetMaterial != nullptr)
                lastTargetMaterial->tint /= 2.0f;
            lastTargetMaterial = mat;
            lastTargetMaterial->tint *= 2.0f;
        }
    }else{
        if (lastTargetMaterial != nullptr){
            lastTargetMaterial->tint /= 2.0f;
            lastTargetMaterial = nullptr;
        }
    }

    auto camInverse = glm::inverse(camera->getViewMatrix());
    auto cam        = camera->getViewMatrix();


    glm::vec3 paimonUp       = glm::vec3(
            paimon->getOwner()->getLocalToWorldMatrix() *
            glm::vec4(0 , 1 , 0 , 0.0)
    );

    glm::vec3 paimonPos       = paimon->getOwner()->getWorldPosition();
    glm::vec3 paimonViewPos   = glm::vec3(cam * glm::vec4(paimonPos , 1.0));
    paimonUp = glm::normalize(paimonUp);
    glm::vec3 paimonViewUp   = glm::vec3(cam * glm::vec4(paimonUp , 0.0));
    paimonViewUp = glm::normalize(paimonViewUp);

    if (paimon->ground == nullptr){
        paimon->ground = level->findBlockNear(paimonViewPos , paimonViewUp);
        if (paimon->ground){
            Events::onPaimonEnter(paimon->ground);
            Events::onPaimonEnterWorld();
            if (paimon->ground->getOwner()->name == "the_winning_block"){
                won = true;
            }
        }

    }

    if (paimon->ground == nullptr){
        std::cout << "Failed to find initial ground for paimon" << std::endl;
        return;
    }

    if (returnToBlockCenter){
        auto myBlock = level->getBlockPositionWorld(paimon->ground);
        auto pos1 = glm::vec3(camera->getViewMatrix() * glm::vec4(paimon->getOwner()->localTransform.position , 1.0));
        auto pos2 = glm::vec3((cam * glm::vec4(myBlock , 1.0))) + paimonViewUp * PAIMON_TO_BLOCK_OFFSET;
        auto diff = pos2 - pos1;
        diff.z = 0;
        pos1.z = glm::max(pos1.z , pos2.z + PAIMON_TO_BLOCK_OFFSET);
        pos1 += glm::normalize(diff) * paimon->speed * deltaTime;
        paimon->getOwner()->localTransform.position = glm::vec3(glm::inverse(camera->getViewMatrix()) * glm::vec4(pos1 , 1.0));
        update_angle(paimon, camera, diff , deltaTime);
        auto dis = level->getPaimonDistanceToGround2D(myBlock , paimonPos , paimonUp);
        if (dis <= BLOCK_REACH_MAX_DIFF){
            // std::cout << "Return to Center" << std::endl;
            returnToBlockCenter = false;
            paimon->getOwner()->localTransform.position = glm::vec3(glm::inverse(camera->getViewMatrix()) * glm::vec4(pos2 , 1.0));
        }

        return;
    }

    if (currentTarget && nextBlock){
        auto dis = level->getPaimonDistanceToGround2D(nextBlockPosition , paimonPos , paimonUp);
        if (dis <= BLOCK_REACH_MAX_DIFF){
            //block reached
            auto route = level->findRoute(paimon->ground , currentTarget);
            if (route.size() > 1){
                nextBlock = route[1].ground;
                nextBlockPosition =  camInverse * glm::vec4(route[1].fakePosition , 1.0);
            } else {
                // std::cout << "Reached end" << std::endl;
                paimon->getOwner()->localTransform.position = nextBlockPosition + glm::vec3(0,1,0) * PAIMON_TO_BLOCK_OFFSET;
                currentTarget = nullptr;
                nextBlock = nullptr;
            }
        }else{
            auto myBlock = level->getBlockPositionWorld(paimon->ground);
            auto myBlockView = cam * glm::vec4(myBlock , 1.0);
            auto nextBlockRoute = level->findRoute(paimon->ground , currentTarget);
            if (nextBlockRoute.empty()){
                // std::cout << "Path cut" << std::endl;
                currentTarget = nullptr;
                nextBlock = nullptr;
                returnToBlockCenter = true;
                nextBlockPosition = myBlock;
                return;
            }

            float distance = level->getPaimonDistanceToGround2D(myBlock , paimonPos , paimonUp);
            if (distance >= BLOCK_WIDTH){
                our::Events::onPaimonExit(paimon->ground);
                paimon->ground = nextBlock;
                our::Events::onPaimonEnter(paimon->ground);
                if (paimon->ground->getOwner()->name == "the_winning_block"){
                    won = true;
                }
            }

            auto pos1 = glm::vec3(camera->getViewMatrix() * glm::vec4(paimon->getOwner()->localTransform.position , 1.0));
            auto pos2 = glm::vec3((cam * glm::vec4(nextBlockPosition , 1.0))) + paimonViewUp * PAIMON_TO_BLOCK_OFFSET;
            auto diff = pos2 - pos1;
            diff.z = 0;
            pos1.z = glm::max(myBlockView.z , pos2.z) + PAIMON_TO_BLOCK_OFFSET;
            diff   = glm::normalize(diff);
            pos1  += diff * paimon->speed * deltaTime;
            paimon->getOwner()->localTransform.position = glm::vec3(camInverse * glm::vec4(pos1 , 1.0));

            update_angle(paimon, camera, diff , deltaTime);
        }
        return;
    }

    orbitalCameraComponent->inputEnabled = true;
    if (app->getMouse().isPressed(0) && target != nullptr) { //left click
        auto route = level->findRoute(paimon->ground, target);
        if (route.size() > 1){
            currentTarget = target;
            nextBlock = route[1].ground;
            nextBlockPosition =  camInverse * glm::vec4(route[1].fakePosition , 1.0);
            orbitalCameraComponent->inputEnabled = false;
        }
    }
}

void our::PaimonMovement::update_angle(our::Paimon *paimon, our::CameraComponent *camera, glm::vec3 diff, float deltaTime) {
    auto diff3D = glm::inverse(camera->getViewMatrix()) * glm::vec4(diff, 0.0);
    // std::cout << "X: " << diff3D.x << " , Y: " << diff3D.y << " , Z: " << diff3D.z << std::endl;
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

void our::PaimonMovement::init(our::Application *a) {
    this->paimon = nullptr;
    this->camera = nullptr;
    this->currentTarget = nullptr;
    this->nextBlock = nullptr;
    this->lastTargetMaterial = nullptr;
    this->orbitalCameraComponent = nullptr;
    this->returnToBlockCenter = false;
    app = a;
    our::GroundSystem::setPaimonController(this);
}

void our::PaimonMovement::onGroundMoved(our::Ground *g, glm::vec3 delta) {
    if (paimon == nullptr) return;
    if (nextBlock == g){
        nextBlockPosition += delta;
    }
    if (paimon->ground == g){
        //std::cout << "Moving Paimon" << std::endl;
        paimon->getOwner()->localTransform.position += delta;
    }
}
