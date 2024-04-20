//
// Created by xAbdoMo on 4/20/2024.
//

#include "PaimonIdle.hpp"
#include "../deserialize-utils.hpp"

namespace our {
    void PaimonIdle::deserialize(const nlohmann::json &data) {
        duration = data.value("duration" , duration);
        width    = data.value("width" , width);
        height   = data.value("height" , width);
        top      = data.value("top" , top);
        forward  = data.value("forward" , forward);

    }
} // our