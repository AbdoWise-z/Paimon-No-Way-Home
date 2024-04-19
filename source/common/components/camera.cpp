#include "camera.hpp"
#include "../ecs/entity.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 

namespace our {
    // Reads camera parameters from the given json object
    void CameraComponent::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;
        std::string cameraTypeStr = data.value("cameraType", "orthographic");//orthographic perspective
        if(cameraTypeStr == "orthographic"){
            cameraType = CameraType::ORTHOGRAPHIC;
        } else {
            cameraType = CameraType::PERSPECTIVE;
        }
        near = data.value("near", 0.01f);
        far = data.value("far", 100.0f);
        fovY = data.value("fovY", 90.0f) * (glm::pi<float>() / 180);
        orthoHeight = data.value("orthoHeight", 1.0f);
    }

    // Creates and returns the camera view matrix
    glm::mat4 CameraComponent::getViewMatrix() const {
        auto owner = getOwner();
        auto M = owner->getLocalToWorldMatrix();
        //TODO: (Req 8) Complete this function
        //HINT:
        // In the camera space:
        // - eye is the origin (0,0,0)
        // - center is any point on the line of sight. So center can be any point (0,0,z) where z < 0. For simplicity, we let center be (0,0,-1)
        // - up is the direction (0,1,0)
        // but to use glm::lookAt, we need eye, center and up in the world state.
        // Since M (see above) transforms from the camera to thw world space, you can use M to compute:
        // - the eye position which is the point (0,0,0) but after being transformed by M
        // - the center position which is the point (0,0,-1) but after being transformed by M
        // - the up direction which is the vector (0,1,0) but after being transformed by M
        // then you can use glm::lookAt

        auto eye    = glm::vec4(0,0,0 , 1.0);
        auto look   = glm::vec4(0,0,-1 , 1.0);
        auto up     = glm::vec4(0,1,0, 0.0);

        eye    = M * eye;
        look   = M * look;
        up     = M * up;

        glm::mat4 V = glm::lookAt(
                 glm::vec3(eye.x , eye.y , eye.z),
                 glm::vec3(look.x , look.y , look.z),
                 glm::vec3(up.x , up.y , up.z)
        );

        return V;
    }

    // Creates and returns the camera projection matrix
    // "viewportSize" is used to compute the aspect ratio

    glm::mat4 CameraComponent::getProjectionMatrix(glm::ivec2 windowSize) const {
        glm::mat4 P = glm::mat4(1.0f);
        float aspectRatio = static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y);
        switch (cameraType) {
            case CameraType::PERSPECTIVE:
                P = glm::perspective(fovY, aspectRatio, near, far);
                break;
            case CameraType::ORTHOGRAPHIC: {

                float viewPortHeight = 50.0f; // this value is set to give me the desired height of the orthographic projection
                float viewPortWidth = viewPortHeight * aspectRatio;
                float left, right, bottom, top;
                if (aspectRatio >= 1.0f) {
                    // Window is wider than tall, adjust horizontal bounds
                    left = -viewPortWidth / 2.0f;
                    right = viewPortWidth / 2.0f;
                    bottom = -viewPortHeight / (2.0f * aspectRatio);
                    top = viewPortHeight / (2.0f * aspectRatio);
                } else {
                    // Window is taller than wide, adjust vertical bounds
                    left = -viewPortWidth / (2.0f * aspectRatio);
                    right = viewPortWidth / (2.0f * aspectRatio);
                    bottom = -viewPortHeight / 2.0f;
                    top = viewPortHeight / 2.0f;
                }

                // Set the near and far plane distances
                float near = -1000.0f;
                float far = 1000.0f;

                P = glm::ortho(left, right, bottom, top, near, far);
                break;
            }
                break;
        }
    return P;
    }
}