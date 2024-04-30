#pragma once

#include <application.hpp>

#include <ecs/world.hpp>
#include <systems/forward-renderer.hpp>
#include <systems/free-camera-controller.hpp>
#include <systems/movement.hpp>
#include <asset-loader.hpp>
#include "systems/paimon-idle.hpp"
#include "systems/Level-mapping.hpp"
#include "systems/orbital-camera-controller.hpp"
#include "systems/paimon-movement.hpp"
#include "systems/collision.hpp"
#include <random>
#include "audio/audio.hpp"
#include <irrKlang.h>


#include "systems/CollectablesSystem.h"
#include "systems/state-system.hpp"

using namespace irrklang;
// This state shows how to use the ECS framework and deserialization.
class Playstate: public our::State {

    our::CollisionSystem collisionSystem;
    our::World world;
    our::ForwardRenderer renderer;
    our::FreeCameraControllerSystem cameraController;
    our::MovementSystem movementSystem;
    our::PaimonIdleSystem paimonIdleSystem;
    our::LevelMapping levelMapping;
    our::OrbitalCameraControllerSystem orbitalCameraControllerSystem;
    our::PaimonMovement paimonMovement;
    our::AudioPlayer* audioPlayer = our::AudioPlayer::getInstance();

    our::StateSystem stateSystem;
    our::CollectablesSystem collectablesSystem;

    our::Texture2D* mora_tex;
    our::Texture2D* game_over_tex;
    // size of framebuffer
    glm::ivec2 size;
    // count of mora
    int mora_count = 0;

    bool gameLost = false;

    bool gameWon = false; //this value should be returned from a system like paimon movement i guess

    int remainingTime = 20;

    float time_counter = 0;

    void initHUD() {
        mora_tex = our::texture_utils::loadImage("assets/textures/mora_icon.png");
        game_over_tex = our::texture_utils::loadImage("assets/textures/game_over.png");
    }

    void drawMoraCount() {
        ImGui::Begin("mora_count" , nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollWithMouse
            | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);

        ImGui::SetWindowPos({30,30});
        ImGui::SetWindowSize({100,40});

        GLuint mora_id = mora_tex->getOpenGLName();
        ImGui::SetCursorPos({0,0});
        ImGui::Image((void*)mora_id,{40,40},{0,1},{1,0});

        ImGui::SetCursorPos({50,0});
        ImGui::SetWindowFontScale(3);
        ImGui::Text(std::to_string(mora_count).c_str());
        ImGui::End();
    }

    void drawTimer() {
        if(time_counter > 1) {
            remainingTime--;
            time_counter = 0;
        }
        if(remainingTime <= 0) {
            gameLost = true;
            remainingTime = 0;
        }

        ImGui::Begin("timer" , nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollWithMouse
            | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);

        ImGui::SetWindowPos({570,30});
        ImGui::SetWindowSize({140,40});

        ImGui::SetCursorPos({18,0});
        ImGui::SetWindowFontScale(3);

        std::string seconds = std::to_string(remainingTime % 60);
        if(remainingTime % 60 <= 9) seconds = "0" + seconds;
        std::string minutes = std::to_string((remainingTime - remainingTime % 60) / 60);
        if((remainingTime - remainingTime % 60) / 60 <= 9) minutes = "0" + minutes;
        std::string timer = minutes + ":" + seconds;

        ImGui::Text(timer.c_str());

        ImGui::End();

    }

    void drawGameLost() {
        static float tempx = 0;
        static float tempy = 0;
        static float posx = 0;
        static float posy = 0;
        auto key = getApp()->getKeyboard();
        if(key.isPressed(GLFW_KEY_1)) {
            tempx +=0.5f;
            std::cout<<"sizex=" <<tempx<<std::endl;
        }
        if(key.isPressed(GLFW_KEY_2)) {
            tempy +=0.5f;
            std::cout<<"sizey=" <<tempx<<std::endl;
        }
        if(key.isPressed(GLFW_KEY_3)) {
            tempx -=0.5f;
            std::cout<<"sizex=" <<tempx<<std::endl;
        }
        if(key.isPressed(GLFW_KEY_4)) {
            tempy -=0.5f;
            std::cout<<"sizey=" <<tempx<<std::endl;
        }
        if(key.isPressed(GLFW_KEY_5)) {
            posx +=0.5f;
            std::cout<<"posx=" <<posx<<std::endl;
        }
        if(key.isPressed(GLFW_KEY_6)) {
            posy +=0.5f;
            std::cout<<"posy=" <<posy<<std::endl;
        }
        if(key.isPressed(GLFW_KEY_7)) {
            posx -=0.5f;
            std::cout<<"posx=" <<posx<<std::endl;
        }
        if(key.isPressed(GLFW_KEY_8)) {
            posy -=0.5f;
            std::cout<<"posy=" <<posy<<std::endl;
        }
        ImGui::Begin("game_over" , nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollWithMouse
            | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar
            | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);

        ImGui::SetWindowPos({0,0});
        ImGui::SetWindowSize({1280,720});

        GLuint id = game_over_tex->getOpenGLName();
        ImGui::SetCursorPos({0,0});
        ImGui::Image((void*)id,{1280,450},{0,1},{1,0});

        ImGui::SetWindowFontScale(3);

        ImGui::SetCursorPos({520,450});
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 0.75f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.5f, 1.0f));
        if(ImGui::Button("Restart",{240, 0})) {
            //TODO: restart level
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();


        ImGui::SetCursorPos({520,550});
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.75f, 0.0f, 0.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.0f, 0.0f, 1.0f));
        if(ImGui::Button("Exit",{240, 0})) {
            //TODO: go to level-menu state
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();

        ImGui::End();
    }

    void drawGameWon() {

    }

    void drawHUD() {
        drawMoraCount();
        drawTimer();
        if(gameLost) drawGameLost();
        if(gameWon) drawGameWon();
    }

    void destroyHUD() {
        delete mora_tex;
        gameLost = false;
        gameWon = false;
    }

    void onImmediateGui() override {
        ImGui::ShowDemoWindow(nullptr);
        drawHUD();
    }
    void onInitialize() override {
        // First of all, we get the scene configuration from the app config
        auto& config = getApp()->getConfig()["scene"];
        // If we have assets in the scene config, we deserialize them
        if(config.contains("assets")){
            our::deserializeAllAssets(config["assets"]);
        }
        // If we have a world in the scene config, we use it to populate our world
        if(config.contains("world")){
            world.deserialize(config["world"]);
        }

        // We initialize the camera controller system since it needs a pointer to the app
        cameraController.enter(getApp());
        // Then we initialize the renderer
        size = getApp()->getFrameBufferSize();
        initHUD();
        collectablesSystem.init();
        renderer.initialize(size, config["renderer"]);
        our::Events::Init(getApp() , &world);
        stateSystem.init(&world);
        levelMapping.init(getApp() , &world);
        orbitalCameraControllerSystem.init(getApp());
        paimonMovement.init(getApp());
        collisionSystem.init(getApp());
//        auto audioAsset =  our::AssetLoader<std::pair<std::string, float>>::get("audio1");
//        if (audioAsset) {
//            auto music = audioAsset->first;
//            auto volume = audioAsset->second;
//            //std::cout<< music<<std::endl;
//            std::cout<< volume<<std::endl;
//
//            if (!music.empty()) {
//                audioPlayer->playSound(music.c_str(), true, volume); // Play a sound with volume 0.5
//            }
//        }else{
//            std::cout<< "audio is not found" <<std::endl;
//        }

    }

    void onDraw(double deltaTime) override {
        time_counter += (float)deltaTime;

        // Here, we just run a bunch of systems to control the world logic
        our::Events::Update((float) deltaTime);
        stateSystem.update(&world , (float) deltaTime);
        movementSystem.update(&world, (float)deltaTime);
        cameraController.update(&world, (float)deltaTime);
        paimonIdleSystem.update(&world, (float)deltaTime);
        orbitalCameraControllerSystem.update(&world , (float) deltaTime);

        levelMapping.update();
        paimonMovement.update(&world , &levelMapping, (float) deltaTime);

        mora_count += collisionSystem.update(&world);

        // And finally we use the renderer system to draw the scene
        renderer.render(&world);

        // Get a reference to the keyboard object
        auto& keyboard = getApp()->getKeyboard();

        if(keyboard.justPressed(GLFW_KEY_ESCAPE)){
            // If the escape  key is pressed in this frame, go to the play state
            getApp()->changeState("level-menu");
        }

//        auto mouse = getApp()->getMouse();
//        auto audioAsset =  our::AssetLoader<std::pair<std::string, float>>::get("audio2");
//
//        if (audioAsset) {
//            auto music = audioAsset->first;
//            auto volume = audioAsset->second;
//            auto isplaying = audioPlayer->isPlaying(music.c_str());
//
//            if (!music.empty() && mouse.isPressed(0) && ! isplaying) {
//                audioPlayer->playSound(music.c_str(), false, volume);
//            }
//        }else{
//            std::cout<< "audio is not found" <<std::endl;
//        }

        world.deleteMarkedEntities();
    }

    void onDestroy() override {
        destroyHUD();
        collectablesSystem.destroy();
        // Don't forget to destroy the renderer
        renderer.destroy();
        // On exit, we call exit for the camera controller system to make sure that the mouse is unlocked
        cameraController.exit();
        // Clear the world
        world.clear();
        // and we delete all the loaded assets to free memory on the RAM and the VRAM
        our::clearAllAssets();
    }
};