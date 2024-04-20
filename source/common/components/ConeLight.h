//
// Created by xAbdoMo on 4/17/2024.
//

#ifndef GFX_LAB_CONELIGHT_H
#define GFX_LAB_CONELIGHT_H

#include "ecs/component.hpp"
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"

namespace our {

    class ConeLight : public Component {
    public:
        float intensity = 1;
        glm::vec3 color = glm::vec3(1,1,1);
        glm::vec3 direction = glm::vec3(0,0,-1);
        glm::vec2 range = glm::vec2(0.5,1);
        int smoothing = 0;

        //used by the renderer
        glm::vec3 worldPosition;
        glm::vec3 worldDirection;
        Decay lightDecay;

        // The ID of this component type is "ConeLight"
        static std::string getID() { return "Cone Light"; }

        // Reads linearVelocity & angularVelocity from the given json object
        void deserialize(const nlohmann::json& data) override;
    };

} // our

#endif //GFX_LAB_CONELIGHT_H
