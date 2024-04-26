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

#include <glm/gtx/intersect.hpp>
#include <queue>
#include <iostream>

#define PAIMON_TO_BLOCK_OFFSET 1.0f
#define PAIMON_TO_BLOCK_DIST   1.2f
#define UP_TO_UP_ALIGNMENT     0.999f

#define TYPE2_BLOCK_MAX_DISTANCE    0.010
#define TYPE2_DIRECTION_ALIGNMENT   0.999
#define BLOCK_WIDTH            1.0f

#define PUSH(i, k) if (k.first >= 0) {groundMap[i].push_back(k); if (!visitedBlocks[k.first]) next.push(k.first);}

namespace our{

    class LevelMapping {
    private:

        [[nodiscard]] inline int findBlockNear(const glm::vec3& paimonPos,const glm::vec3& paimonUp,const std::vector<bool>& visited) const{
            for (int i = 0;i < blocks.size();i++){
                if (visited[i]) continue;

                auto block = blocks[i];
                if (glm::dot(paimonUp, block.up) < UP_TO_UP_ALIGNMENT) continue;

                auto dis = block.position - paimonPos + paimonUp * PAIMON_TO_BLOCK_OFFSET;
                if (glm::dot(dis , dis) < PAIMON_TO_BLOCK_DIST){ //a near block
                    return i;
                }
            }
            return -1;
        }

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

        inline int getPaimonInitialPosition(const glm::vec3& pos, const glm::vec3& up, std::vector<bool>& visited){
            int initial;
            //add the initial block
            if (paimon->ground == nullptr) {
                initial = findBlockNear(
                        pos,
                        up,
                        visited
                );
                if (initial == -1) return -1;

                paimon->ground = blocks[initial].ground;
            }else{
                initial = -1;
                for (int i = 0;i < blocks.size();i++){
                    if (blocks[i].ground == paimon->ground){
                        initial = i;
                        break;
                    }
                }
            }

            return initial;
        }

        std::vector<GroundBlock> blocks;
        GroundLinks groundMap;

    public:
        Application* app{};
        Paimon* paimon{};
        CameraComponent* camera{};
        Entity* marker{};
        World* world{};

        void init(Application* a, World* mWorld){
            this->app = a;
            this->world = mWorld;
            update();
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
        float getPaimonDistanceToGround(glm::vec3 block_pos){
            auto PV = camera->getViewMatrix();
            glm::vec3 paimonUp       = glm::vec3(
                    PV * paimon->getOwner()->getLocalToWorldMatrix() *
                    glm::vec4(0 , 1 , 0 , 0.0)
            );

            paimonUp = glm::normalize(paimonUp);

            glm::vec3 paimonPos       = glm::vec3(
                    PV * paimon->getOwner()->getLocalToWorldMatrix() *
                    glm::vec4(0 , 0 , 0 , 1.0)
            );

            auto dis = block_pos - paimonPos + paimonUp * PAIMON_TO_BLOCK_OFFSET;
            return glm::length(dis);
        }

        float getPaimonDistanceToGround2D(glm::vec3 block_pos){
            auto PV = camera->getViewMatrix();
            glm::vec3 paimonUp       = glm::vec3(
                    PV * paimon->getOwner()->getLocalToWorldMatrix() *
                    glm::vec4(0 , 1 , 0 , 0.0)
            );

            paimonUp = glm::normalize(paimonUp);

            glm::vec3 paimonPos       = glm::vec3(
                    PV * paimon->getOwner()->getLocalToWorldMatrix() *
                    glm::vec4(0 , 0 , 0 , 1.0)
            );

            paimonPos.z = 0;
            block_pos.z = 0;
            paimonUp.z  = 0;

            auto dis = block_pos - paimonPos + paimonUp * PAIMON_TO_BLOCK_OFFSET;
            return glm::length(dis);
        }


        std::vector<RoutePart> findRoute(Ground* target){
            if (target == nullptr) return {};
            auto PV = camera->getViewMatrix();

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
            paimonViewUp = glm::normalize(paimonViewUp);

            std::vector<bool> visited;
            for (auto k : blocks) visited.push_back(false);

            int initial = getPaimonInitialPosition(paimonPosition, paimonViewUp, visited);

            if (initial == -1) return {};
            if (blocks[initial].ground == target) return {};


            visited.clear();
            for (auto k : blocks) visited.push_back(false);

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
            marker->localTransform.position.x = pos.x;
            marker->localTransform.position.y = pos.y;
            marker->localTransform.position.z = pos.z;

            float minDis = 1e15;
            int hitI     = -1;
            int index    = 0;
            for (auto k: blocks){
                float val = 0;

                //currently, I'm doing Sphere ray cast cuz it just easier ..
                if (glm::intersectRaySphere(
                        glm::vec3(vsVector),
                        glm::vec3(0,0,1),
                        k.position,
                        2.0f,
                        val
                        )){

                    if (abs(k.position.z) < minDis){
                        minDis = abs(k.position.z);
                        hitI = index;
                    }
                }

                index++;
            }

            if (hitI != -1) ((DefaultMaterial*) blocks[hitI].et->getComponent<MeshRendererComponent>()->material)->tint = glm::vec4(0, 2 , 2 , 1);
            if (hitI == -1) return nullptr;

            return blocks[hitI].ground;
        }

        std::vector<GroundBlock>& getBlocks(){
            return blocks;
        }

        void update() {
            std::vector<Ground*> ground_blocks;
            std::vector<bool> visitedBlocks;

            blocks.clear();
            groundMap.clear();

            //first we need to get all of our objects ready
            for (auto k : world->getEntities()){
                if (k->name == "marker"){
                    marker = k;
                }
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
            int initial = getPaimonInitialPosition(paimonPosition, paimonViewUp, visitedBlocks);

            if (initial >= 0)
                next.push(initial);


            glm::vec3 camForward = glm::vec3(
                    glm::mat4(1) * glm::vec4(0,0,1,0)
                    );

            while (!next.empty()){
                auto index = next.front(); next.pop();
                visitedBlocks[index] = true;

                GroundBlock g = blocks[index];

                auto l = findBlockAlongDirection2(left     , g.position , paimonViewUp, index );
                auto r = findBlockAlongDirection2(-left    , g.position , paimonViewUp, index );
                auto f = findBlockAlongDirection2(forward  , g.position , paimonViewUp, index );
                auto b = findBlockAlongDirection2(-forward , g.position , paimonViewUp, index );

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

            //try to find route to the last block
            auto test = findRoute(ScreenToGroundCast(app->getMouse().getMousePosition().x , app->getMouse().getMousePosition().y));
            for (auto k : test){
                ((DefaultMaterial*) blocks[k.blockIndex].et->getComponent<MeshRendererComponent>()->material)->tint = glm::vec4(0, 0 , 1 , 1);
            }
        }
    };
}

#endif //GFX_LAB_LEVEL_MAPPING_HPP
