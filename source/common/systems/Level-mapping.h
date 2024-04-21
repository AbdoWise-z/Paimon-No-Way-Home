//
// Created by xAbdoMo on 4/20/2024.
//

#ifndef GFX_LAB_LEVEL_MAPPING_H
#define GFX_LAB_LEVEL_MAPPING_H

#include "ecs/world.hpp"
#include "components/Paimon.hpp"
#include "components/Ground.hpp"
#include "components/camera.hpp"
#include "application.hpp"

#include <glm/gtx/intersect.hpp>
#include <queue>
#include <iostream>

#define PAIMON_TO_BLOCK_OFFSET 1.0f
#define PAIMON_TO_BLOCK_DIST   0.4f
#define UP_TO_UP_ALIGNMENT     0.999f

#define DIRECTION_ALIGNMENT    0.99f

#define BLOCK_MAX_DISTANCE     2.2f
#define BLOCK_MIN_DISTANCE     1.8000f

#define PUSH(i, k) if (k >= 0) {groundMap[i].push_back(k); if (!visitedBlocks[k]) next.push(k);}

namespace our{
    typedef std::pair<int, int> link;

    struct GroundBlock{
        glm::vec3 position;
        glm::vec3 up;
        Entity* et;
        Ground* ground;
    };

    typedef std::unordered_map<int , std::vector<int>> GroundLinks;

    class LevelMapping {
    private:

        inline int findBlockNear(glm::vec3& paimon, glm::vec3& paimonUp, std::vector<GroundBlock>& mBlocks , std::vector<bool>& visited) const{
            for (int i = 0;i < mBlocks.size();i++){
                if (visited[i]) continue;

                auto block = mBlocks[i];
                if (glm::dot(paimonUp, block.up) < UP_TO_UP_ALIGNMENT) continue;

                auto dis = block.position - paimon + paimonUp * PAIMON_TO_BLOCK_OFFSET;
                if (enableVisualTricks) dis.z = 0; //camera should be looking along the z-axis
                if (glm::dot(dis , dis) < PAIMON_TO_BLOCK_DIST){ //a near block
                    return i;
                }
            }
            return -1;
        }

        [[nodiscard]] inline int findBlockAlongDirection(
                const glm::vec3& direction,
                const glm::vec3& position,
                const glm::vec3& up,
                const std::vector<GroundBlock>& blocks
                ) const{
            int res = -1;
            float minDis = 1e10;

            for (int i = 0;i < blocks.size();i++){
                auto block = blocks[i];

                if (glm::dot(block.up , up) < UP_TO_UP_ALIGNMENT) continue;

                auto P0 = block.position;
                auto P1 = position;

                auto dis = P0 - P1;
                auto distance = glm::dot(dis , dis);

                if (distance == 0) continue; // self

                auto directionAlignment = glm::dot(glm::normalize(dis) , glm::normalize(direction));
                distance = glm::sqrt(distance);

                if ((directionAlignment > DIRECTION_ALIGNMENT) && (distance < minDis) && (distance >= BLOCK_MIN_DISTANCE)){ //a near block
                    minDis = distance;
                    res = i;
                    continue;
                }

                if (enableVisualTricks){
                    /*
                     * L0 = P0 + cam_dir * S0
                     * L1 = P1 + dir     * S1
                     *
                     * X0 = X1 -> cam_dir.x * S0 - dir.x * S1 = P0.x - P1.x
                     * Y0 = Y1 -> cam_dir.y * S0 - dir.y * S1 = P0.y - P1.y
                     *
                     * solve for S0 , S1
                     * sub for z to check
                     *
                     * [cx dx  ppx]     [cx    dx                  ppx                 ]
                     * [cy dy  ppy] --> [0     dy - dx * cy / cx   ppy - ppx * cy / cx ]
                     *
                     * S1 = A0 / A1
                     * S0 = (ppx - dx * A0 / A1) / cx
                     *
                     * but we actually only have cam_dir.z
                     * so the equations are much simpler
                     * */


                    float s1, s0 = 0;
                    if (direction.x != 0){
                        s1 = (P0.x - P1.x) / direction.x;
                        s0 = (P1.z + direction.z * s1 - P0.z);
                    } else if (direction.y != 0){
                        s1 = (P0.y - P1.y) / direction.y;
                        s0 = (P1.z + direction.z * s1 - P0.z);
                    }

                    P0 = P0 + glm::vec3(0 , 0 , 1) * s0;
                    dis = P0 - P1;
                    distance = glm::dot(dis , dis);
                    if (distance == 0) continue; // self
                    directionAlignment = glm::dot(glm::normalize(dis) , glm::normalize(direction));
                    distance = glm::sqrt(distance);
                    if ((directionAlignment > DIRECTION_ALIGNMENT) && (distance < minDis) && (distance >= BLOCK_MIN_DISTANCE)){ //a near block
                        minDis = distance;
                        res = i;
                    }
                }
            }

            if (res != -1){
                //validate the distance
                if (minDis > BLOCK_MAX_DISTANCE){
                    return -1;
                }
            }

            return res;
        }

        inline bool canStand(glm::vec3& paimon, glm::vec3& ground , glm::vec3& paimonUp, glm::vec3& groundUp) const{
            if (glm::dot(groundUp , paimonUp) < UP_TO_UP_ALIGNMENT) return false;
            auto dis = ground - paimon + paimonUp * PAIMON_TO_BLOCK_OFFSET;
            if (enableVisualTricks) dis.z = 0;
            return glm::dot(dis , dis) < PAIMON_TO_BLOCK_DIST;
        }


        std::vector<GroundBlock> blocks;
        GroundLinks groundMap;

    public:
        Application* app{};

        bool enableVisualTricks = true; //ignore the y-axis when doing calculations

        void init(Application* a){
            this->app = a;
        }

        void update(World *world, float deltaTime) {
            Paimon* paimon = nullptr;
            std::vector<Ground*> ground_blocks;
            CameraComponent* camera = nullptr;
            std::vector<bool> visitedBlocks;

            blocks.clear();
            groundMap.clear();

            //first we need to get all of our objects ready
            for (auto k : world->getEntities()){
                if (paimon == nullptr) paimon = k->getComponent<Paimon>();
                if (camera == nullptr) camera = k->getComponent<CameraComponent>();
                auto g = k->getComponent<Ground>();
                if (g){
                    ground_blocks.emplace_back(g);
                    ((DefaultMaterial*) k->getComponent<MeshRendererComponent>()->material)->tint = glm::vec4(1, 0.5 , 0.5 , 1);
                }
            }

            if (!camera || !paimon) return;

            auto PV = camera->getViewMatrix();
            for (auto k : ground_blocks){
                Entity* et = k->getOwner();
                auto localToWorld = et->getLocalToWorldMatrix();
                glm::vec4 pos =  localToWorld * glm::vec4(0, 0, 0 , 1.0);
                pos = PV * pos;
                glm::vec4 up  = PV * et->getLocalToWorldMatrix() * glm::vec4(k->up , 0.0);
                GroundBlock b = {
                        glm::vec3(pos),
                        glm::vec3(up),
                        et,
                        k
                };

                blocks.emplace_back(b);
                visitedBlocks.push_back(false);
            }

            glm::vec3 paimonUp       = glm::vec3(
                    paimon->getOwner()->getLocalToWorldMatrix() *
                    glm::vec4(0 , 1 , 0 , 0.0)
                    );

            glm::vec3 paimonPosition = glm::vec3(
                    PV *
                    paimon->getOwner()->getLocalToWorldMatrix() *
                    glm::vec4(0,0,0 , 1.0)
                    );

            glm::vec3 paimonViewUp = glm::vec3(PV * glm::vec4(paimonUp , 0.0));
            glm::vec3 left         = glm::vec3(PV * glm::vec4(1,0,0 , 0.0));
            glm::vec3 top          = glm::vec3(PV * glm::vec4(0,1,0 , 0.0));
            glm::vec3 forward      = glm::vec3(PV * glm::vec4(0,0,1 , 0.0));

            paimonViewUp = glm::normalize(paimonViewUp);
            left         = glm::normalize(left);
            forward      = glm::normalize(forward);
            top          = glm::normalize(top);


            std::queue<int> next;
            //add the initial block
            auto initial = findBlockNear(
                    paimonPosition ,
                    paimonViewUp ,
                    blocks ,
                    visitedBlocks
                    );

            if (initial >= 0)
                next.push(initial);


            glm::vec3 camForward = glm::vec3(
                    glm::mat4(1) * glm::vec4(0,0,1,0)
                    );

            while (!next.empty()){
                auto index = next.front(); next.pop();
                visitedBlocks[index] = true;

                GroundBlock g = blocks[index];

                int l = findBlockAlongDirection(left     , g.position , paimonViewUp , blocks);
                int r = findBlockAlongDirection(-left    , g.position , paimonViewUp , blocks);
                int f = findBlockAlongDirection(forward  , g.position , paimonViewUp , blocks);
                int b = findBlockAlongDirection(-forward , g.position , paimonViewUp , blocks);

                PUSH(index , l);
                PUSH(index , r);
                PUSH(index , f);
                PUSH(index , b);
            }

            auto it = groundMap.begin();
            while (it != groundMap.end()){
                ((DefaultMaterial*) blocks[it->first].et->getComponent<MeshRendererComponent>()->material)->tint = glm::vec4(0.5, 1 , 0.5 , 1);
                it++;
            }
        }
    };
}

#endif //GFX_LAB_LEVEL_MAPPING_H
