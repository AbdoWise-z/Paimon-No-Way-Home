//
// Created by xAbdoMo on 4/20/2024.
//

#ifndef GFX_LAB_ORBITAL_CAMERA_CONTROLLER_HPP
#define GFX_LAB_ORBITAL_CAMERA_CONTROLLER_HPP

#include "ecs/world.hpp"
#include "components/camera.hpp"
#include "components/OrbitalCameraComponent.h"
#include "application.hpp"
#include <glm/gtx/euler_angles.hpp>

namespace our{
    class OrbitalCameraControllerSystem{
        Application* app;


        [[nodiscard]] static inline float slow_in_slow_out(float a) {
            return (float) (1.0 - glm::pow(1.0 - a , 1.4));
        }

    public:
        void init(Application* a){
            app = a;
        }

        void update(World* world, float deltaTime) {
            CameraComponent* camera = nullptr;
            OrbitalCameraComponent *controller = nullptr;
            for(auto entity : world->getEntities()){
                camera = entity->getComponent<CameraComponent>();
                controller = entity->getComponent<OrbitalCameraComponent>();
                if(camera && controller) break;
            }


            if(!(camera && controller)) return;

            //set the camera
            glm::vec3 rotation = glm::radians(controller->BaseAngle) +
                                 glm::vec3(0, 1 , 0) * glm::two_pi<float>() / controller->Divisions * (controller->_currentPos - controller->_switchDirection * slow_in_slow_out(controller->_switchProgress));
            glm::vec4 initial = glm::vec4(0 , 0 , 1 , 0);
            glm::mat4 rotationMat = glm::yawPitchRoll(rotation.y, rotation.x, rotation.z);
            auto pos = rotationMat * initial;

            camera->getOwner()->localTransform.position = pos * controller->Distance;
            camera->getOwner()->localTransform.rotation = rotation;

            if (controller->_switchProgress > 0.0001){
                controller->_switchProgress -= deltaTime / controller->switchSpeed;
                if (controller->_switchProgress < 0) controller->_switchProgress = 0;
            }else{

                if (app->getKeyboard().isPressed(GLFW_KEY_Q)){
                    controller->_currentPos--;
                    controller->_switchDirection = -1;
                    controller->_switchProgress = 1;
                }

                if (app->getKeyboard().isPressed(GLFW_KEY_E)){
                    controller->_currentPos++;
                    controller->_switchDirection = 1;
                    controller->_switchProgress = 1;
                }

            }
        }
    };
}
#endif //GFX_LAB_ORBITAL_CAMERA_CONTROLLER_HPP
