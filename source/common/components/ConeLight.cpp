//
// Created by xAbdoMo on 4/17/2024.
//

#include "ConeLight.h"
#include "../deserialize-utils.hpp"

namespace our {
    void ConeLight::deserialize(const nlohmann::json &data) {
        if(!data.is_object()) return;
        intensity        = data.value("intensity", intensity);
        direction        = data.value("direction", direction);
        ambientColor     = data.value("ambientColor", ambientColor);
        specularColor    = data.value("specularColor", specularColor);
        diffuseColor     = data.value("diffuseColor", diffuseColor);
        smoothing        = data.value("smoothing", smoothing);
        range            = data.value("range", range);
        attenuation      = data.value("attenuation", attenuation);
    }
} // our