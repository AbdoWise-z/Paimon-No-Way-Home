//
// Created by xAbdoMo on 4/20/2024.
//

#include "OrbitalCameraComponent.h"
#include "../deserialize-utils.hpp"
#include "systems/events-system-controller.hpp"

namespace our {
    void OrbitalCameraComponent::deserialize(const nlohmann::json &data) {
        if (!data.is_object()) return;
        Distance     = data.value("Distance" , Distance);
        OrthoZoom    = data.value("OrthoZoom" , Distance);
        switchSpeed  = data.value("switchSpeed" , Distance);
        Divisions    = data.value("Divisions" , Distance);
        speed        = data.value("speed" , Distance);
        follow       = data.value("follow" , follow);
        inputEnabled = data.value("inputEnabled" , inputEnabled);
        BaseAngle    = data.value("BaseAngle" , BaseAngle);
        BasePosition = data.value("BasePosition" , BasePosition);
        switches     = data.value("switches" , switches);
    }

    void OrbitalCameraComponent::trigger(const nlohmann::json &data) {
        if (!data.is_object()) return;
        std::string action = data.value("action" , "none");
        if (action == "switch_change"){
            switches += data.value("change" , 0);
        }

        if (action == "divisions_change"){
            Divisions += data.value("change" , 0.0f);
        }

        if (action == "set_speed"){
            speed = data.value("value" , speed);
        }

        if (action == "move_left"){
            _currentPos++;
            _switchDirection = 1;
            _switchProgress = 1;
            Events::onPaimonCameraChange(getOwner()->name);
        }

        if (action == "move_right"){
            _currentPos--;
            _switchDirection = -1;
            _switchProgress = 1;
            Events::onPaimonCameraChange(getOwner()->name);
        }

        if (action == "follow"){
            follow.push_back(data.value("target" , ""));
        }



        if (action == "unfollow"){
            auto it = std::find(follow.begin(), follow.end(), data.value("target" , ""));
            if (it != follow.end()){
                follow.erase(it);
            }
        }
    }

    std::string OrbitalCameraComponent::getReceiverID() {
        return OrbitalCameraComponent::getID();
    }
} // our