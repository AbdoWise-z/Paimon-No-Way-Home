//
// Created by xAbdoMo on 4/27/2024.
//

#ifndef GFX_LAB_EVENTS_SYSTEM_CONTROLLER_HPP
#define GFX_LAB_EVENTS_SYSTEM_CONTROLLER_HPP

#include "application.hpp"
#include "ecs/world.hpp"
#include "components/Ground.hpp"

namespace our::Events{

    void Init(Application* app, World* world);
    void Update(float deltaTime);

    void onPaimonEnter(Ground* g);
    void onPaimonExit(Ground* g);

    void onPaimonInteract(const std::string& name);
    void onPaimonPickMora(const std::string& more_name);
    void onPaimonCameraChange(const std::string& name);


    void onPaimonEnterWorld();

}


#endif //GFX_LAB_EVENTS_SYSTEM_CONTROLLER_HPP
