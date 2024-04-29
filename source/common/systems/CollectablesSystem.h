#pragma once
#include "components/Mora.hpp"
#include <queue>

namespace our
{
    class CollectablesSystem {

        Paimon* paimon;

        float accumalatedTime = 0;

        TexturedMaterial* moraMaterial;
        Mesh* rectangle;
        float translation = 0;
        float pos = 0;
        float speed = 336;
        std::queue<Mora*> currentlyPlaying;
        float animationTime = 0;
        float time = 0;
        float temp = 0;

    public:

        void init() {
            moraMaterial = new TexturedMaterial();
            moraMaterial->shader = new ShaderProgram();
            moraMaterial->shader->attach("assets/shaders/textured.vert", GL_VERTEX_SHADER);
            moraMaterial->shader->attach("assets/shaders/textured.frag", GL_FRAGMENT_SHADER);
            moraMaterial->shader->link();
            moraMaterial->texture = our::texture_utils::loadImage("assets/textures/mora_icon.png");
            moraMaterial->tint = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            moraMaterial->pipelineState.blending.enabled = true;
            rectangle = new Mesh({
                            {{0.0f, 0.0f, 0.0f}, {255, 255, 255, 255}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
                            {{1.0f, 0.0f, 0.0f}, {255, 255, 255, 255}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
                            {{1.0f, 1.0f, 0.0f}, {255, 255, 255, 255}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
                            {{0.0f, 1.0f, 0.0f}, {255, 255, 255, 255}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
                            },{
                                0, 1, 2, 2, 3, 0,
                            });
        }
        // This should be called every frame to update all entities that are mora
        void update(World* world, int& count, float deltaTime, glm::ivec2 size, float time,Application* app) {

            glm::mat4 VP = glm::ortho(0.0f, (float)size.x, (float)size.y, 0.0f, 1.0f, -1.0f);
            glm::mat4 M = glm::scale(glm::mat4(1.0f), glm::vec3(60.0f, 60.0f, 1.0f));
            glm::mat4 T_init = glm::translate(M, glm::vec3(10.0f, 5.2f, 0.0f));

            moraMaterial->tint = glm::vec4(glm::smoothstep(0.00f, 2.00f, time));

            if(!currentlyPlaying.empty()) {
                animationTime += deltaTime;
                if(animationTime - 2 < 0.02 && animationTime - 2 > 0) {
                    animationTime = 0;
                    currentlyPlaying.pop();
                }
                float t = animationTime * speed * 0.0135f;
                glm::mat4 T = glm::translate(T_init, glm::vec3(-t, -t / 2.5, 0.99f));
                moraMaterial->setup();
                moraMaterial->shader->set("transform", VP*T);
                rectangle->draw();
            }

            accumalatedTime +=deltaTime;
            // For each entity in the world
            for(auto entity : world->getEntities()){
                if(paimon == nullptr) paimon = entity->getComponent<Paimon>();
            }
            if(!paimon) {
                std::cout<< "no paimon for ya";
                return;
            }
            for(auto entity : world->getEntities()) {
                Mora* mora = entity->getComponent<Mora>();
                if(mora) {
                    //TODO : collision system
                }
            }
        }
        void destroy() {
            delete moraMaterial->texture;
            delete moraMaterial->shader;
            delete moraMaterial;
            delete rectangle;
        }
    };

}
