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

enum GameState{PLAYING, WON, LOST};
// This state shows how to use the ECS framework and deserialization.
class Playstate: public our::State {

    // systems
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
    // textures
    our::Texture2D* mora_tex;
    our::Texture2D* game_over_tex;
    our::Texture2D* game_won_tex;
    our::Texture2D* paimon_icon;
    our::Texture2D* button_style;
    //fonts
    ImFont* genhsinFont = nullptr;
    // size of framebuffer
    glm::ivec2 size;
    // count of mora
    int mora_count = 0;
    // current game state
    GameState gameState = PLAYING; // this value should be returned from a system like paimon movement i guess
    // audios
    std::pair<std::string,float>* game_over_audio;
    std::pair<std::string,float>* game_won_audio;
    // time remaining to lose
    int remainingTime = 3; //TODO: this value should be loaded from each level's json file
    // HUD parameters
    float time_counter = 0;
    ImVec2 windowSize;
    float fontScale = 3.0f;
    float buttonWidth = 240.0f;
    float buttonPosx = 0.0f;
    float buttonCurvature = 100.0f;
    ImVec2 button_style_size = {50.0f, 50.0f};
    ImVec2 button_style_pos_offset = {200.0f, 19.0f};
    std::vector<float> hudPadding = {30.0f, 30.0f, 30.0f, 30.0f}; // {top, left , bottom , right}
    bool showMenu = false;

    void initHUD() {
        windowSize.x = size.x;
        windowSize.y = size.y;
        buttonPosx = (windowSize.x - buttonWidth) / 2;
        mora_tex = our::texture_utils::loadImage("assets/textures/mora_icon.png");
        game_over_tex = our::texture_utils::loadImage("assets/textures/game_over.png");
        game_won_tex = our::texture_utils::loadImage("assets/textures/game_won.png");
        paimon_icon = our::texture_utils::loadImage("assets/textures/paimon_icon.png");
        button_style = our::texture_utils::loadImage("assets/textures/button_style.png");

        ImGuiIO& io = ImGui::GetIO();

        io.Fonts->AddFontDefault();

        // Load a custom font from a file
        const char* font_filename = "assets/fonts/genshin.ttf";
        genhsinFont = io.Fonts->AddFontFromFileTTF(font_filename, 13.0f);


        // Build the font atlas (important for rendering)
        io.Fonts->Build();
    }

    void drawMoraCount() {
        ImGui::Begin("mora_count" , nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollWithMouse
            | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);

        float width = 100.0f;
        float height = 40.0f;
        ImGui::SetWindowPos({hudPadding[1],hudPadding[0]});
        ImGui::SetWindowSize({width,height});

        GLuint mora_id = mora_tex->getOpenGLName();
        ImGui::SetCursorPos({0,0});
        ImGui::Image((void*)mora_id,{40,40},{0,1},{1,0});

        ImGui::SetCursorPos({50,0});
        ImGui::SetWindowFontScale(fontScale);
        ImGui::Text(std::to_string(mora_count).c_str());
        ImGui::End();
    }

    void drawTimer() {
        if(time_counter > 1) {
            remainingTime--;
            time_counter = 0;
        }
        if(remainingTime <= 0) {
            gameState = LOST;
            remainingTime = 0;
        }

        ImGui::Begin("timer" , nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollWithMouse
            | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);

        float width = 140.0f;
        float height = 40.0f;
        ImGui::SetWindowPos({(windowSize.x  - width) / 2,hudPadding[0]});
        ImGui::SetWindowSize({width,height});

        ImVec2 timerInnerPadding = {18,0};
        ImGui::SetCursorPos(timerInnerPadding);
        ImGui::SetWindowFontScale(fontScale);

        std::string seconds = std::to_string(remainingTime % 60);
        if(remainingTime % 60 <= 9) seconds = "0" + seconds;
        std::string minutes = std::to_string((remainingTime - remainingTime % 60) / 60);
        if((remainingTime - remainingTime % 60) / 60 <= 9) minutes = "0" + minutes;
        std::string timer = minutes + ":" + seconds;

        ImGui::Text(timer.c_str());

        ImGui::End();

    }

    void drawEndGame() {
        static bool playSound = false;

        if(gameState == LOST) {
            if (game_over_audio && !playSound) {
                auto music = game_over_audio->first;
                auto volume = game_over_audio->second;
                if (!music.empty()) {
                    audioPlayer->playSound(music.c_str(), false, volume); // Play a sound with volume 0.5
                    playSound = true;
                }
            }
        }else {
            if (game_won_audio && !playSound) {
                auto music = game_won_audio->first;
                auto volume = game_won_audio->second;
                if (!music.empty()) {
                    audioPlayer->playSound(music.c_str(), false, volume); // Play a sound with volume 0.5
                    playSound = true;
                }
            }
        }

        ImGui::Begin("game_end" , nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollWithMouse
            | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar
            | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);

        ImGui::SetWindowPos({0,0});
        ImGui::SetWindowSize(windowSize);

        GLuint id = gameState == LOST? game_over_tex->getOpenGLName() : game_won_tex->getOpenGLName();

        float imageWidth = windowSize.x;
        float imageHeight = 450.0f;
        ImGui::SetCursorPos({0,0});
        ImGui::Image((void*)id,{imageWidth,imageHeight},{0,1},{1,0});

        ImGui::SetWindowFontScale(fontScale);

        ImGui::SetCursorPos({buttonPosx,imageHeight});

        std::string buttonLabel = gameState == LOST? "Restart" : "Continue";
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.24f, 0.24f, 0.24f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.24f, 0.24f, 0.24f, 0.9f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.24f, 0.24f, 0.24f, 1.0f));
        if(ImGui::Button(buttonLabel.c_str(),{buttonWidth, 0})) {
            if(gameState == LOST){
                //TODO: restart level
            }else {
                //TODO: go to next level
            }
            playSound = false;
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();

        GLuint style_id = button_style->getOpenGLName();
        ImGui::SetCursorPos({buttonPosx + button_style_pos_offset.x,imageHeight - button_style_pos_offset.y});
        ImGui::Image((void*)style_id,button_style_size,{0,1},{1,0});

        ImGui::SetCursorPos({buttonPosx,imageHeight + 100.0f});

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.85f, 0.2f, 0.15f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.85f, 0.2f, 0.15f, 0.9f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.85f, 0.2f, 0.15f, 1.0f));
        if(ImGui::Button("Exit",{buttonWidth, 0})) {
            getApp()->changeState("main-menu");
            playSound = false;
        }

        ImGui::SetCursorPos({buttonPosx + button_style_pos_offset.x,imageHeight + 100.0f - button_style_pos_offset.y});
        ImGui::Image((void*)style_id,button_style_size,{0,1},{1,0});

        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();

        ImGui::End();
    }

    void drawMenu() {
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.5f));

        ImGui::Begin("Press Esc to close." , &showMenu, ImGuiWindowFlags_NoTitleBar| ImGuiWindowFlags_NoScrollWithMouse
            | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar
            | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
        ImGui::SetWindowPos({0,0});
        ImGui::SetWindowSize({windowSize.x + 10,windowSize.y + 10}); // +10 to get rid of these borders lol

        ImGui::SetWindowFontScale(fontScale);

        float topPadding = 230.0f;
        ImGui::SetCursorPos({buttonPosx,topPadding});

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.24f, 0.24f, 0.24f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.24f, 0.24f, 0.24f, 0.9f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.24f, 0.24f, 0.24f, 1.0f));
        if(ImGui::Button("Levels",{buttonWidth, 0})) {
            //TODO: go to levels menu state
        }

        GLuint style_id = button_style->getOpenGLName();
        ImGui::SetCursorPos({buttonPosx + button_style_pos_offset.x,topPadding - button_style_pos_offset.y});
        ImGui::Image((void*)style_id,button_style_size,{0,1},{1,0});

        ImGui::SetCursorPos({buttonPosx,topPadding + 120.0f});

        if(ImGui::Button("Options",{buttonWidth, 0})) {
            //looks good tho
        }
        ImGui::SetCursorPos({buttonPosx + button_style_pos_offset.x,topPadding + 120.0f - button_style_pos_offset.y});
        ImGui::Image((void*)style_id,button_style_size,{0,1},{1,0});

        ImGui::SetCursorPos({buttonPosx,topPadding + 240.0f});

        if(ImGui::Button("Main Menu",{buttonWidth, 0})) {
            getApp()->changeState("main-menu");

        }

        ImGui::SetCursorPos({buttonPosx + button_style_pos_offset.x,topPadding + 240.0f - button_style_pos_offset.y});
        ImGui::Image((void*)style_id,button_style_size,{0,1},{1,0});

        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();

        float imageWidth = 105.0f;
        float imageHeight = 140.0f;
        ImGui::SetCursorPos({windowSize.x - imageWidth - hudPadding[3],windowSize.y - imageHeight - hudPadding[2]});
        GLuint id = paimon_icon->getOpenGLName();
        ImGui::Image((void*)id,{imageWidth,imageHeight},{0,1},{1,0});

        ImGui::End();

        ImGui::PopStyleColor();

    }

    void drawHUD() {

        ImGui::PushFont(genhsinFont);

        drawMoraCount();
        drawTimer();
        if(gameState != PLAYING) drawEndGame();
        if(showMenu && gameState == PLAYING) drawMenu();

        ImGui::PopFont();
    }

    void destroyHUD() {
        delete mora_tex;
        delete game_over_tex;
        delete game_won_tex;
        delete paimon_icon;
        delete button_style;

        //ImGuiIO& io = ImGui::GetIO();
        //io.Fonts->Clear();
        //delete genhsinFont;

        gameState = PLAYING;
        showMenu = false;
    } //TODO: delete things

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
        remainingTime = config["game"].value("remainingTime",0);
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
        auto audioAsset =  our::AssetLoader<std::pair<std::string, float>>::get("audio1");
        game_over_audio =  our::AssetLoader<std::pair<std::string, float>>::get("audio3");
        game_won_audio =  our::AssetLoader<std::pair<std::string, float>>::get("audio2");
        if (audioAsset) {
            auto music = audioAsset->first;
            auto volume = audioAsset->second;
            //std::cout<< music<<std::endl;
            std::cout<< volume<<std::endl;

            if (!music.empty()) {
                audioPlayer->playSound(music.c_str(), true, volume); // Play a sound with volume 0.5
            }
        }else{
            std::cout<< "audio is not found" <<std::endl;
        }
    }

    void onDraw(double deltaTime) override {

        if(!showMenu) time_counter += (float)deltaTime;

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
            showMenu = !showMenu;
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