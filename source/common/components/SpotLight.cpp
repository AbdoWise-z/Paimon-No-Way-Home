//
// Created by xAbdoMo on 4/17/2024.
//

#include "SpotLight.h"
#include "../deserialize-utils.hpp"

namespace our {
    void SpotLight::deserialize(const nlohmann::json &data) {
        if(!data.is_object()) return;
        intensity = data.value("intensity", intensity);
        color     = data.value("color", color);
    }
} // our