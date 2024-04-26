//
// Created by xAbdoMo on 4/20/2024.
//

#ifndef GFX_LAB_GROUND_HPP
#define GFX_LAB_GROUND_HPP

#include "ecs/component.hpp"
#include "glm/vec3.hpp"

namespace our {

    enum GroundType {
        SIMPLE = 0,
        CURVED = 1,
        STAIRS = 2,
    };

    class Ground: public Component {
    public:

        GroundType variant = SIMPLE;
        glm::vec3 up = glm::vec3(0,1,0);
        static std::string getID() { return "Ground"; }
        void deserialize(const nlohmann::json& data) override;
    };

} // our

#endif //GFX_LAB_GROUND_HPP
