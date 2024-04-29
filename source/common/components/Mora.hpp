//
// Created by HIMA on 4/29/2024.
//

#ifndef GFX_LAB_MORA_HPP
#define GFX_LAB_MORA_HPP
#pragma once

#include "../ecs/component.hpp"

#include <glm/glm.hpp>

namespace our {


    class Mora : public Component {
    public:
        glm::vec3 offset = glm::vec3(0,1,0);
        static std::string getID() { return "Mora"; }
        void deserialize(const nlohmann::json& data) override;
    };

}
#endif //GFX_LAB_MORA_HPP
