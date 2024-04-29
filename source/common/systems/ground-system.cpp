//
// Created by xAbdoMo on 4/28/2024.
//
#include "ground-system.hpp"
#include <iostream>

static our::PaimonMovement* paimon;
void our::GroundSystem::setPaimonController(PaimonMovement * p) {
    paimon = p;
}

void our::GroundSystem::onGroundMoved(our::Ground *ground, glm::vec3 world_delta) {
    if (paimon != nullptr){
        paimon->onGroundMoved(ground , world_delta);
    }else{
        std::cout << "Error: No PaimonMovement attached on when : our::GroundSystem::onGroundMoved" << std::endl;
    }
}

