//
// Created by xAbdoMo on 4/20/2024.
//

#include "Ground.hpp"
#include "deserialize-utils.hpp"

namespace our {
    void Ground::deserialize(const nlohmann::json &data) {
        variant = data.value("variant" , variant);
        up   = data.value("up" , up);
    }
} // our