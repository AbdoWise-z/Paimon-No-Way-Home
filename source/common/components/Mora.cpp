//
// Created by HIMA on 4/29/2024.
//
#include "Mora.hpp"
#include "../ecs/entity.hpp"

namespace our {
    void Mora::deserialize(const nlohmann::json& data){
        if (!data.is_object()) return;

        type = static_cast<MORA_TYPE>(data.value("mora_type", 0));
    }
}