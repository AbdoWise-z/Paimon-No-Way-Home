#pragma once

#include "../ecs/entity.hpp"
#include "camera.hpp"
#include "mesh-renderer.hpp"
#include "free-camera-controller.hpp"
#include "movement.hpp"
#include "DirectionalLight.hpp"
#include "SpotLight.h"
#include "ConeLight.h"
#include "PaimonIdle.hpp"
#include "Paimon.hpp"
#include "Mora.h"
#include "Ground.hpp"
#include "OrbitalCameraComponent.h"
#include "event-controller.h"
#include "components/actions/StateAnimator.h"

namespace our {

    // Given a json object, this function picks and creates a component in the given entity
    // based on the "type" specified in the json object which is later deserialized from the rest of the json object
    inline void deserializeComponent(const nlohmann::json& data, Entity* entity){
        std::string type = data.value("type", "");
        Component* component = nullptr;
        //TODO: (Req 8) Add an option to deserialize a "MeshRendererComponent" to the following if-else statement
        if(type == CameraComponent::getID()){
            component = entity->addComponent<CameraComponent>();
        } else if (type == FreeCameraControllerComponent::getID()) {
            component = entity->addComponent<FreeCameraControllerComponent>();
        } else if (type == MovementComponent::getID()) {
            component = entity->addComponent<MovementComponent>();
        } else if (type == MeshRendererComponent::getID()){
            component = entity->addComponent<MeshRendererComponent>();
        } else if (type == DirectionalLight::getID()){
            component = entity->addComponent<DirectionalLight>();
        } else if (type == SpotLight::getID()){
            component = entity->addComponent<SpotLight>();
        } else if (type == ConeLight::getID()){
            component = entity->addComponent<ConeLight>();
        } else if (type == PaimonIdle::getID()){
            component = entity->addComponent<PaimonIdle>();
        } else if (type == Paimon::getID()){
            component = entity->addComponent<Paimon>();
        } else if (type == Ground::getID()){
            component = entity->addComponent<Ground>();
        } else if (type == OrbitalCameraComponent::getID()){
            component = entity->addComponent<OrbitalCameraComponent>();
        } else if (type == EventController::getID()){
            component = entity->addComponent<EventController>();
        } else if (type == StateAnimator::getID()){
            component = entity->addComponent<StateAnimator>();
        }else if(type == Mora::getID()) {
            component = entity->addComponent<Mora>();
        }

        if (component) component->deserialize(data);
    }

}