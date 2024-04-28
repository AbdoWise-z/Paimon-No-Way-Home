//
// Created by xAbdoMo on 4/23/2024.
//

#ifndef GFX_LAB_PAIMON_MOVEMENT_HPP
#define GFX_LAB_PAIMON_MOVEMENT_HPP

#include "Level-mapping.hpp"
#include "material/material.hpp"
#include "components/OrbitalCameraComponent.h"

#define BLOCK_REACH_MAX_DIFF 0.1

namespace our {
    class PaimonMovement {
    private:
        Application* app{};
        DefaultMaterial* lastTargetMaterial = nullptr;
        Ground* currentTarget;
        Ground* nextBlock;
        glm::vec3 nextBlockPosition;
        bool returnToBlockCenter = false;
        Paimon* paimon = nullptr;
        CameraComponent* camera = nullptr;
        OrbitalCameraComponent* orbitalCameraComponent = nullptr;


        static inline void update_angle(Paimon* paimon, CameraComponent* camera, glm::vec3 diff , float deltaTime);

    public:
        void init(Application* a);
        void update(World *world, LevelMapping* level, float deltaTime);
        void onGroundMoved(Ground* g, glm::vec3 delta);
    };
}

#endif //GFX_LAB_PAIMON_MOVEMENT_HPP
