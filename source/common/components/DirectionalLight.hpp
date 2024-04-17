//
// Created by xAbdoMo on 4/17/2024.
//

#ifndef GFX_LAB_DIRECTIONALLIGHT_HPP
#define GFX_LAB_DIRECTIONALLIGHT_HPP

#include "ecs/component.hpp"
#include "glm/vec3.hpp"

namespace our {

    class DirectionalLight : public Component {
    public:
        glm::vec3 direction = glm::vec3(0,1,0);
        float intensity = 1;
        glm::vec3 color = glm::vec3(1,1,1);

        // The ID of this component type is "DirectionalLight"
        static std::string getID() { return "Directional Light"; }

        // Reads linearVelocity & angularVelocity from the given json object
        void deserialize(const nlohmann::json& data) override;
    };

} // our

#endif //GFX_LAB_DIRECTIONALLIGHT_HPP
