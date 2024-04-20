//
// Created by xAbdoMo on 4/20/2024.
//

#ifndef GFX_LAB_PAIMONIDLE_HPP
#define GFX_LAB_PAIMONIDLE_HPP

#include "ecs/component.hpp"
#include "glm/vec3.hpp"

namespace our {

    class PaimonIdle: public Component {
    public:
        float duration = 4; //in sec
        glm::vec3 top = glm::vec3(0, 1 , 0);
        glm::vec3 forward = glm::vec3(0, 0 , 1);
        float width = 0.5;
        float height = 1;

        // The ID of this component type is "Mesh Renderer"
        static std::string getID() { return "Paimon Idle"; }

        // Receives the mesh & material from the AssetLoader by the names given in the json object
        void deserialize(const nlohmann::json& data) override;
    };

} // our

#endif //GFX_LAB_PAIMONIDLE_HPP
