//
// Created by xAbdoMo on 4/17/2024.
//

#include "DirectionalLight.hpp"
#include "../deserialize-utils.hpp"

namespace our {
    void DirectionalLight::deserialize(const nlohmann::json &data) {
        if(!data.is_object()) return;
        intensity = data.value("intensity", intensity);
        direction = data.value("direction", direction);
        color     = data.value("color", color);
    }
} // our