//
// Created by xAbdoMo on 4/17/2024.
//

#include "ConeLight.h"
#include "../deserialize-utils.hpp"

namespace our {
    void ConeLight::deserialize(const nlohmann::json &data) {
        if(!data.is_object()) return;
        intensity = data.value("intensity", intensity);
        direction = data.value("direction", direction);
        color     = data.value("color", color);
        smoothing = data.value("smoothing", smoothing);
        range     = data.value("range", range);
    }
} // our