//
// Created by xAbdoMo on 4/20/2024.
//

#ifndef GFX_LAB_PAIMON_IDLE_HPP
#define GFX_LAB_PAIMON_IDLE_HPP

#include "ecs/world.hpp"
#include "components/PaimonIdle.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "iostream"

namespace our
{
    class PaimonIdleSystem{
    public:
        float time = 0;
        void update(World* world, float deltaTime) {
            // First of all, we search for an entity containing both a CameraComponent and a FreeCameraControllerComponent
            // As soon as we find one, we break

            PaimonIdle* paimon = nullptr;
            time += deltaTime;

            for(auto entity : world->getEntities()){
                paimon = entity->getComponent<PaimonIdle>();
                if(paimon) break;
            }

            if (paimon == nullptr) return;


            while (time > paimon->duration){
                time -= paimon->duration;
            }

            auto et = paimon->getOwner();

            auto mat = et->getLocalToWorldMatrix();
            mat = glm::transpose(glm::inverse(mat));

            auto top4 = mat * glm::vec4(paimon->top , 0.0);
            auto for4 = mat * glm::vec4(paimon->forward , 0.0);

            auto right = glm::cross(
                    glm::vec3(top4) ,
                    glm::vec3(for4)
                    );

            right = glm::normalize(right);
            auto top = glm::normalize(glm::vec3(top4));

            et->localTransform.position  =
                    right  * (float) glm::sin(time / paimon->duration * 4 * glm::pi<float>()) * paimon->width +
                    top    * (float) glm::sin(time / paimon->duration * 2 * glm::pi<float>()) * paimon->height
                    ;

        }
    };
}
#endif //GFX_LAB_PAIMON_IDLE_HPP
