//
// Created by xAbdoMo on 4/20/2024.
//

#include "OrbitalCameraComponent.h"
#include "../deserialize-utils.hpp"

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
    }
} // our