//
// Created by xAbdoMo on 4/20/2024.
//

#ifndef GFX_LAB_ORBITALCAMERACOMPONENT_H
#define GFX_LAB_ORBITALCAMERACOMPONENT_H

#include "ecs/component.hpp"
#include "glm/vec3.hpp"

namespace our {

    class OrbitalCameraComponent: public Component {
    public:
        float Distance = 50;
        float OrthoZoom = 10;
        float switchSpeed = 0.8; //in sec
        float speed = 4;
        float Divisions = 8;
        bool inputEnabled = true;

        float _switchProgress = 1;
        float _currentPos = 0;
        float _switchDirection = 1;
        glm::vec3 _currentLocation = glm::vec3(0,0,0);

        glm::vec3 BaseAngle = glm::vec3(-45 , -45 , 0);
        glm::vec3 BasePosition = glm::vec3(0 , 0 , 0);
        std::vector<std::string> follow;

        static std::string getID() { return "Orbital Camera Component"; }
        void deserialize(const nlohmann::json& data) override;
    };

} // our

#endif //GFX_LAB_ORBITALCAMERACOMPONENT_H
