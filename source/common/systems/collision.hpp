//
// Created by HIMA on 4/29/2024.
//

#ifndef GFX_LAB_COLLISION_HPP
#define GFX_LAB_COLLISION_HPP

#include "../ecs/world.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include "../ecs/entity.hpp"
#include "../components/Mora.hpp"
#include <iostream>
#include <application.hpp>
#include <systems/forward-renderer.hpp>
#include "audio/audio.hpp"
namespace our
{
    class CollisionSystem
    {
        Application *app;
        glm::vec3 paimonPos;
        Entity *mora = nullptr;
        Entity *paimon = nullptr;
        int counter = 0;

    public:
        void init(Application *app);
        void update(World *world, ForwardRenderer *forwardRenderer, bool &gameOverflag, AudioPlayer *audioPlayer);
        void checkGameOver(bool gameOverflag);
        void exit();
    };
}

#endif //GFX_LAB_COLLISION_HPP
