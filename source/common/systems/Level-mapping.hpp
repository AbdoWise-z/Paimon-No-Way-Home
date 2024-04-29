//
// Created by xAbdoMo on 4/20/2024.
//

#ifndef GFX_LAB_LEVEL_MAPPING_HPP
#define GFX_LAB_LEVEL_MAPPING_HPP

#include "ecs/world.hpp"
#include "components/Paimon.hpp"
#include "components/Ground.hpp"
#include "components/camera.hpp"
#include "application.hpp"
#include "events-system-controller.hpp"

#include <glm/gtx/intersect.hpp>
#include <queue>
#include <iostream>

#define PAIMON_TO_BLOCK_OFFSET 1.0f
#define PAIMON_TO_BLOCK_DIST   1.2f
#define UP_TO_UP_ALIGNMENT     0.999f

#define TYPE2_BLOCK_MAX_DISTANCE    0.010
#define TYPE2_DIRECTION_ALIGNMENT   0.999
#define BLOCK_WIDTH            1.0f

#define PUSH(i, k) if (k.first >= 0) {groundMap[i].push_back(k);}

#define v3AB(a , b , v) v = glm::vec3(a * b * glm::vec4(v , 1.0))

namespace our{

    class LevelMapping {
    private:
        [[nodiscard]] inline std::pair<int,glm::vec3> findBlockAlongDirection2(
                const glm::vec3& direction,
                const glm::vec3& position,
                const glm::vec3& up,
                const int& ignoreIndex
        ) const{
            int ret = -1;
            float mDepth = 1e10;
            glm::vec3 block_position;
            for (int i = 0;i < blocks.size();i++){
                if (i == ignoreIndex) continue;
                auto block = blocks[i];

                if (glm::dot(block.up , up) < UP_TO_UP_ALIGNMENT) continue;
                auto P0 = block.position;
                auto P1 = position + glm::normalize(direction) * 2.0f;
                auto depth = abs(P0.z); //distance from cam

                auto dot = glm::dot(glm::normalize(P1 - P0) , glm::vec3(0,0,1));
                auto len = glm::length(P1 - P0);
                if (glm::abs(dot) > TYPE2_DIRECTION_ALIGNMENT || len <= TYPE2_BLOCK_MAX_DISTANCE){
                    if (glm::abs(depth) < glm::abs(mDepth)){
                        ret = i;
                        mDepth = depth;
                        block_position = P1;
                        block_position.z = glm::max(P1.z , P0.z);
                    }
                }
            }
            return {ret , block_position};
        }

        std::vector<GroundBlock> blocks;
        GroundLinks groundMap;

    public:
        Application* app{};
        CameraComponent* camera{};
        World* world{};

        void init(Application* a, World* mWorld){
            this->camera = nullptr;
            this->blocks.clear();
            this->groundMap.clear();
            this->app = a;
            this->world = mWorld;
            update();
        }

        [[nodiscard]] inline Ground* findBlockNear(const glm::vec3& paimonPos,const glm::vec3& paimonUp) const{
            for (auto block : blocks){
                if (glm::dot(paimonUp, block.up) < UP_TO_UP_ALIGNMENT) continue;

                auto dis = block.position - paimonPos + paimonUp * PAIMON_TO_BLOCK_OFFSET;
                auto len = glm::length(dis);
                if (len < PAIMON_TO_BLOCK_DIST){ //a near block
                    return block.ground;
                }
            }
            return nullptr;
        }

        inline glm::vec3 getBlockPositionWorld(Ground* g){
            if (g == nullptr){
                throw "Ground can't be null";
            }
            return g->getOwner()->getWorldPosition();
        }

        inline glm::vec3 getBlockPosition(Ground* g){
            if (g == nullptr){
                throw "Ground can't be null";
            }

            auto PV = camera->getViewMatrix();
            glm::vec3 pos       = glm::vec3(
                    PV * g->getOwner()->getLocalToWorldMatrix() *
                    glm::vec4(0 , 0 , 0 , 1.0)
            );
            return pos;
        }

        // block_pos in camera space
        float getPaimonDistanceToGround(glm::vec3 block_pos , glm::vec3 paimonPos , glm::vec3 paimonUp){
            auto PV = camera->getViewMatrix();
            paimonUp       = glm::vec3(
                    PV * glm::vec4(paimonUp , 0.0)
            );
            paimonUp = glm::normalize(paimonUp);

            paimonPos     = glm::vec3(
                    PV * glm::vec4(paimonPos , 1.0)
            );

            auto dis = block_pos - paimonPos + paimonUp * PAIMON_TO_BLOCK_OFFSET;
            return glm::length(dis);
        }

        // Calculates paimon distance to the ground from camera perspective
        // block_pos: the block position in world space
        // paimonPos: paimon position in world space
        // paimonUp : a vector pointing from paimon upwards in world space
        float getPaimonDistanceToGround2D(glm::vec3 block_pos , glm::vec3 paimonPos , glm::vec3 paimonUp){
            auto PV = camera->getViewMatrix();
            paimonUp       = glm::vec3(
                    PV * glm::vec4(paimonUp , 0.0)
            );
            paimonUp = glm::normalize(paimonUp);
            paimonPos     = glm::vec3(
                    PV * glm::vec4(paimonPos , 1.0)
            );
            block_pos     = glm::vec3(
                    PV * glm::vec4(block_pos , 1.0)
            );

            paimonPos.z = 0;
            block_pos.z = 0;
            paimonUp.z  = 0;

            auto dis = block_pos - paimonPos + paimonUp * PAIMON_TO_BLOCK_OFFSET;
            return glm::length(dis);
        }


        std::vector<RoutePart> findRoute(Ground* start , Ground* target){
            if (target == nullptr || start == nullptr) {
                std::cout << "Error: Target or Start is equal to null |  target = " << target << " , start = " << start << std::endl;
                return {};
            }

            if (start == target){
                return {};
            }

            auto PV = camera->getViewMatrix();
            std::vector<bool> visited;
            int initial = -1;
            for (int i = 0;i < blocks.size();i++) {
                visited.push_back(false);
                if (blocks[i].ground == start){
                    initial = i;
                }
            }

            if (initial == -1) {
                std::cout << "Error: Failed to find the initial block";
                return {};
            }

            auto root = new PathTreeNode<std::pair<int,glm::vec3>>();
            root->value = {initial , blocks[initial].position};
            root->parent = nullptr;

            std::queue<PathTreeNode<std::pair<int,glm::vec3>>*> next;
            next.push(root);

            std::vector<PathTreeNode<std::pair<int,glm::vec3>>*> allNodes;
            allNodes.emplace_back(root);

            std::vector<std::pair<int,glm::vec3>> path;
            bool path_found = false;

            while (!next.empty()){
                auto v = next.front();
                next.pop();
                for (auto k : groundMap[v->value.first]){
                    if (visited[k.first]) continue;
                    if (blocks[k.first].ground == target){
                        //found it
                        path.push_back(k);
                        while (v){
                            path.push_back(v->value);
                            v = v->parent;
                        }
                        path_found = true;
                    } else {
                        visited[k.first] = true;
                        auto newNode = new PathTreeNode<std::pair<int,glm::vec3>>();
                        newNode->value = k;
                        newNode->parent = v;
                        allNodes.emplace_back(newNode);
                        next.push(newNode);
                    }

                    if (path_found) break;
                }

                if (path_found) break;
            }

            for (auto k : allNodes){
                delete k;
            }

            if (!path_found) return {};

            std::vector<RoutePart> route;

            for (int i = path.size() - 1;i >= 0;i --){
                RoutePart p = {
                        path[i].first,
                        path[i].second,
                        blocks[path[i].first].ground
                };
                route.emplace_back(p);
            }

            return route;
        }

        Ground* ScreenToGroundCast(float screenX, float screenY){
            auto fSx = (float) screenX;
            auto fSy = (float) app->getFrameBufferSize().y - (float) screenY;

            fSx /= (float) app->getFrameBufferSize().x;
            fSx -= 0.5;
            fSx *= 2;

            fSy /= (float) app->getFrameBufferSize().y;
            fSy -= 0.5;
            fSy *= 2;

            //now we have the NDC coords
            glm::vec4 ndcVector = glm::vec4(fSx , fSy , 0 , 1);
            glm::vec4 vsVector  = glm::inverse(camera->getProjectionMatrix(app->getFrameBufferSize())) * ndcVector;

            auto temp = vsVector;
            temp.z = -1;
            glm::vec4 pos = glm::inverse(camera->getViewMatrix()) * vsVector;

            float minDis = 1e15;
            int hitI     = -1;
            int index    = 0;
            for (auto k: blocks){
                float val = 0;

                auto p = glm::vec3(0);
                auto Ma = k.et->getLocalToWorldMatrix();
                auto Mb = camera->getViewMatrix();
                auto v0 = glm::vec3(-1,-1, 1);
                auto v1 = glm::vec3( 1,-1, 1);
                auto v2 = glm::vec3(-1, 1, 1);
                auto v3 = glm::vec3( 1, 1, 1);
                auto v4 = glm::vec3(-1,-1,-1);
                auto v5 = glm::vec3( 1,-1,-1);
                auto v6 = glm::vec3(-1, 1,-1);
                auto v7 = glm::vec3( 1, 1,-1);
                v3AB(Mb , Ma , v0);
                v3AB(Mb , Ma , v1);
                v3AB(Mb , Ma , v2);
                v3AB(Mb , Ma , v3);
                v3AB(Mb , Ma , v4);
                v3AB(Mb , Ma , v5);
                v3AB(Mb , Ma , v6);
                v3AB(Mb , Ma , v7);

                bool intersect = glm::intersectLineTriangle(
                                glm::vec3(vsVector),
                                glm::vec3(0,0,1),
                                v0,v1,v2,
                                p
                        ) ||
                        glm::intersectLineTriangle(
                                glm::vec3(vsVector),
                                glm::vec3(0,0,1),
                                v2,v1,v3,
                                p
                        ) ||
                        glm::intersectLineTriangle(
                                glm::vec3(vsVector),
                                glm::vec3(0,0,1),
                                v0,v4,v2,
                                p
                        ) ||
                        glm::intersectLineTriangle(
                                glm::vec3(vsVector),
                                glm::vec3(0,0,1),
                                v4,v2,v6,
                                p
                        ) ||
                        glm::intersectLineTriangle(
                                glm::vec3(vsVector),
                                glm::vec3(0,0,1),
                                v6,v2,v7,
                                p
                        ) ||
                        glm::intersectLineTriangle(
                                glm::vec3(vsVector),
                                glm::vec3(0,0,1),
                                v2,v7,v3,
                                p
                        );

                if (intersect){
                    if (abs(k.position.z) < minDis){
                        minDis = abs(k.position.z);
                        hitI = index;
                    }
                }

                index++;
            }

            //if (hitI != -1) ((DefaultMaterial*) blocks[hitI].et->getComponent<MeshRendererComponent>()->material)->tint = glm::vec4(0, 2 , 2 , 1);
            if (hitI == -1) return nullptr;

            return blocks[hitI].ground;
        }

        std::vector<GroundBlock>& getBlocks(){
            return blocks;
        }

        void update() {
            std::vector<Ground*> ground_blocks;

            blocks.clear();
            groundMap.clear();

            //first we need to get all of our objects ready
            for (auto k : world->getEntities()){
                if (camera == nullptr) camera = k->getComponent<CameraComponent>();
                auto g = k->getComponent<Ground>();
                if (g){
                    ground_blocks.emplace_back(g);
                    //((DefaultMaterial*) k->getComponent<MeshRendererComponent>()->material)->tint = glm::vec4(1, 0.5 , 0.5 , 1);
                }
            }

            if (!camera) return;

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
            }

            glm::vec3 left         = glm::vec3(PV * glm::vec4(1,0,0 , 0.0));
            glm::vec3 top          = glm::vec3(PV * glm::vec4(0,1,0 , 0.0));
            glm::vec3 forward      = glm::vec3(PV * glm::vec4(0,0,1 , 0.0));

            left         = glm::normalize(left);
            forward      = glm::normalize(forward);
            top          = glm::normalize(top);

            std::queue<int> next;
            for (int i = 0;i < blocks.size();i++){
                next.push(i);
            }

            while (!next.empty()){
                auto index = next.front(); next.pop();

                GroundBlock g = blocks[index];

                auto l = findBlockAlongDirection2(left     , g.position , top, index );
                auto r = findBlockAlongDirection2(-left    , g.position , top, index );
                auto f = findBlockAlongDirection2(forward  , g.position , top, index );
                auto b = findBlockAlongDirection2(-forward , g.position , top, index );

                PUSH(index , l)
                PUSH(index , r)
                PUSH(index , f)
                PUSH(index , b)
            }


//            auto it = groundMap.begin();
//            while (it != groundMap.end()){
//                ((DefaultMaterial*) blocks[it->first].et->getComponent<MeshRendererComponent>()->material)->tint = glm::vec4(0.5, 1 , 0.5 , 1);
//                it++;
//            }

            //try to find route to the last block
//            auto test = findRoute(ScreenToGroundCast(app->getMouse().getMousePosition().x , app->getMouse().getMousePosition().y));
//            for (auto k : test){
//                ((DefaultMaterial*) blocks[k.blockIndex].et->getComponent<MeshRendererComponent>()->material)->tint = glm::vec4(0, 0 , 1 , 1);
//            }
        }
    };
}

#endif //GFX_LAB_LEVEL_MAPPING_HPP
