//
// Created by xAbdoMo on 4/17/2024.
//

#ifndef GFX_LAB_SPOTLIGHT_H
#define GFX_LAB_SPOTLIGHT_H

#include "ecs/component.hpp"
#include "glm/vec3.hpp"

namespace our {

    class SpotLight: public Component {
    public:
        float intensity = 1;
        glm::vec3 color = glm::vec3(1,1,1);
        glm::vec3 worldPosition;

        // The ID of this component type is "DirectionalLight"
        static std::string getID() { return "Spot Light"; }

        // Reads linearVelocity & angularVelocity from the given json object
        void deserialize(const nlohmann::json& data) override;
    };

} // our

#endif //GFX_LAB_SPOTLIGHT_H
