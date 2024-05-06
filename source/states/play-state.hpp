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

#include "systems/state-system.hpp"
#include "texture/texture-utils.hpp"

using namespace irrklang;

enum GameState{PLAYING, WON, LOST};
// This state shows how to use the ECS framework and deserialization.
class Playstate: public our::State {

    // systems
    our::CollisionSystem collisionSystem;
    our::World world;
    our::ForwardRenderer renderer;
    our::MovementSystem movementSystem;
    our::PaimonIdleSystem paimonIdleSystem;
    our::LevelMapping levelMapping;
    our::OrbitalCameraControllerSystem orbitalCameraControllerSystem;
    our::PaimonMovement paimonMovement;
    our::AudioPlayer* audioPlayer = our::AudioPlayer::getInstance();
    our::StateSystem stateSystem;
    // textures
    our::Texture2D* mora_tex;
    our::Texture2D* game_over_tex;
    our::Texture2D* game_won_tex;
    our::Texture2D* paimon_icon;
    our::Texture2D* button_style;
    our::Texture2D* camera_icon;
    our::Texture2D* angle_icon;
    // size of framebuffer
    glm::ivec2 size;
    // count of mora
    int mora_count = 0;
    // current game state
    GameState gameState = PLAYING; // this value should be returned from a system like paimon movement i guess
    // audios
    std::pair<std::string,float>* game_over_audio;
    std::pair<std::string,float>* game_won_audio;
    ISound* ost;
    // time remaining to lose
    int remainingTime = 3;
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

    our::OrbitalCameraComponent* cameraComponent;

    void initHUD() {
        windowSize.x = size.x;
        windowSize.y = size.y;
        buttonPosx = (windowSize.x - buttonWidth) / 2;

        mora_tex = our::texture_utils::loadImage("assets/textures/mora_icon.png");
        game_over_tex = our::texture_utils::loadImage("assets/textures/game_over.png");
        game_won_tex = our::texture_utils::loadImage("assets/textures/game_won.png");
        paimon_icon = our::texture_utils::loadImage("assets/textures/paimon_icon.png");
        button_style = our::texture_utils::loadImage("assets/textures/button_style.png");

        camera_icon = our::texture_utils::loadImage("assets/textures/icons/camera.png");
        angle_icon  = our::texture_utils::loadImage("assets/textures/icons/angle.png");

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

    void drawGameplayConfigurations(double delta, int camera_count = 1000 , int angle_count = 8) {
        static int prev_camera_count = 0;
        static int prev_angle_count  = 0;
        static float camera_anim = 0;
        static float angle_anim = 0;
        const float camera_anim_duration = 0.4;
        const float angle_anim_duration  = 0.8;

        ImGui::Begin("game_play" , nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollWithMouse
                                             | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);

        if (prev_camera_count != camera_count){
            camera_anim = 1;
            prev_camera_count = camera_count;
        }

        if (prev_angle_count != angle_count){
            angle_anim = 1;
            prev_angle_count = angle_count;
        }

        camera_anim -= delta / camera_anim_duration;
        angle_anim  -= delta / angle_anim_duration;

        if (camera_anim < 0) camera_anim = 0;
        if (angle_anim < 0)  angle_anim = 0;


        ImVec4 text_color_normal = {1 , 1 , 1 , 1};
        ImVec4 text_color_changed = {0 , 1 , 0 , 1};
        ImVec4 total_color_camera = {0,0,0,0};
        total_color_camera.x = text_color_normal.x * (1 - camera_anim) + text_color_changed.x * camera_anim;
        total_color_camera.y = text_color_normal.y * (1 - camera_anim) + text_color_changed.y * camera_anim;
        total_color_camera.z = text_color_normal.z * (1 - camera_anim) + text_color_changed.z * camera_anim;
        total_color_camera.w = text_color_normal.w * (1 - camera_anim) + text_color_changed.w * camera_anim;

        ImVec4 total_color_angle = {0,0,0,0};
        total_color_angle.x = text_color_normal.x * (1 - angle_anim) + text_color_changed.x * angle_anim;
        total_color_angle.y = text_color_normal.y * (1 - angle_anim) + text_color_changed.y * angle_anim;
        total_color_angle.z = text_color_normal.z * (1 - angle_anim) + text_color_changed.z * angle_anim;
        total_color_angle.w = text_color_normal.w * (1 - angle_anim) + text_color_changed.w * angle_anim;


        float width = 130.0f;
        float height = 40.0f * 2;
        ImGui::SetWindowPos({hudPadding[1],hudPadding[0] + 40 + 5});
        ImGui::SetWindowSize({width,height});

        GLuint camera_ic = camera_icon->getOpenGLName();
        ImGui::SetCursorPos({5,2.5});
        ImGui::Image((void*) camera_ic,{35,35},{0,1},{1,0});

        ImGui::PushStyleColor(ImGuiCol_Text , total_color_camera);
        ImGui::SetCursorPos({50,5});
        ImGui::SetWindowFontScale(fontScale * 0.8);
        ImGui::Text("%s", camera_count > 1000 ? "inf" : std::to_string(camera_count).c_str());
        ImGui::PopStyleColor();

        GLuint angle_ic = angle_icon->getOpenGLName();
        ImGui::SetCursorPos({5,2.5 + 40});
        ImGui::Image((void*) angle_ic,{35,35},{0,1},{1,0});

        ImGui::PushStyleColor(ImGuiCol_Text , total_color_angle);
        ImGui::SetCursorPos({50,5 + 40});
        ImGui::SetWindowFontScale(fontScale * 0.8);
        ImGui::Text("%s", std::to_string(angle_count).c_str());
        ImGui::PopStyleColor();

        ImGui::End();
    }

    void drawTimer() {
        if(time_counter > 1 && gameState == PLAYING) {
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
        static bool playHoverSound = false;
        static bool restart_hover = false;
        static bool exit_hover = false;

        if(gameState == LOST) {
            if (game_over_audio && !playSound) {
                audioPlayer->playSound(game_over_audio->first.c_str(), false, game_over_audio->second); // Play a sound with volume 0.5
                playSound = true;
            }
        }else {
            if (game_won_audio && !playSound) {
                audioPlayer->playSound(game_won_audio->first.c_str(), false, game_won_audio->second); // Play a sound with volume 0.5
                playSound = true;
            }
        }

        ImGui::Begin("game_end" , nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollWithMouse
            | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar
            | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);

        ImGui::SetWindowPos({0,0});
        ImGui::SetWindowSize(windowSize);

        GLuint id = gameState == LOST ? game_over_tex->getOpenGLName() : game_won_tex->getOpenGLName();

        float imageWidth = windowSize.x;
        float imageHeight = 450.0f;
        ImGui::SetCursorPos({0,0});
        ImGui::Image((void*) id,{imageWidth,imageHeight},{0,1},{1,0});

        ImGui::SetWindowFontScale(fontScale);

        ImGui::SetCursorPos({buttonPosx,imageHeight});

        std::string buttonLabel = gameState == LOST ? "Restart" : "Continue";
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.24f, 0.24f, 0.24f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.24f, 0.24f, 0.24f, 0.9f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.24f, 0.24f, 0.24f, 1.0f));
        if(ImGui::Button(buttonLabel.c_str(),{buttonWidth, 0})) {
            audioPlayer->playSound(our::press_button_audio.first.c_str(),false, our::press_button_audio.second);
            if(gameState == LOST){
                getApp()->changeState("play");
            }else {
                our::curr_level++;
                our::level_path = getApp()->getConfig()["levels"][our::curr_level % 5].get<std::string>();
                getApp()->changeState("play");
                audioPlayer->stopSound(ost->getSoundSource());
            }
            playSound = false;
        }
        if(ImGui::IsItemHovered() && !playHoverSound && !restart_hover) {
            audioPlayer->playSound(our::hover_button_audio.first.c_str(),false, our::hover_button_audio.second);
            restart_hover = true;
            playHoverSound = true;
        }else if(!ImGui::IsItemHovered() && !exit_hover) {
            restart_hover = false;
            playHoverSound = false;
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
            audioPlayer->playSound(our::press_button_audio.first.c_str(),false, our::press_button_audio.second);
            getApp()->changeState("main-menu");
            playSound = false;
            audioPlayer->stopSound(ost->getSoundSource());
        }
        if(ImGui::IsItemHovered() && !playHoverSound && !exit_hover) {
            audioPlayer->playSound(our::hover_button_audio.first.c_str(),false, our::hover_button_audio.second);
            exit_hover = true;
            playHoverSound = true;
        }else if(!ImGui::IsItemHovered() && !restart_hover) {
            exit_hover = false;
            playHoverSound = false;
        }

        ImGui::SetCursorPos({buttonPosx + button_style_pos_offset.x,imageHeight + 100.0f - button_style_pos_offset.y});
        ImGui::Image((void*)style_id,button_style_size,{0,1},{1,0});

        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();

        ImGui::End();
    }

    void drawMenu() {

        static bool button1_hover = false;
        static bool button2_hover = false;
        static bool button3_hover = false;
        static bool playHoverSound = false;

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
            audioPlayer->playSound(our::press_button_audio.first.c_str(),false, our::press_button_audio.second);
            getApp()->changeState("level-menu");
            audioPlayer->stopSound(ost->getSoundSource());
        }
        if(ImGui::IsItemHovered() && !button1_hover) {
            audioPlayer->playSound(our::hover_button_audio.first.c_str(),false, our::hover_button_audio.second);
            button1_hover = true;
            playHoverSound = true;
        }else if(!ImGui::IsItemHovered() && !button2_hover && !button3_hover) {
            button1_hover = false;
            playHoverSound = false;
        }

        GLuint style_id = button_style->getOpenGLName();
        ImGui::SetCursorPos({buttonPosx + button_style_pos_offset.x,topPadding - button_style_pos_offset.y});
        ImGui::Image((void*)style_id,button_style_size,{0,1},{1,0});

        ImGui::SetCursorPos({buttonPosx,topPadding + 120.0f});

        if(ImGui::Button("Options",{buttonWidth, 0})) {
            //looks good tho
            audioPlayer->playSound(our::press_button_audio.first.c_str(),false, our::press_button_audio.second);
        }
        if(ImGui::IsItemHovered() && !button2_hover) {
            audioPlayer->playSound(our::hover_button_audio.first.c_str(),false, our::hover_button_audio.second);
            button2_hover = true;
            playHoverSound = true;
        }else if(!ImGui::IsItemHovered() && !button1_hover && !button3_hover) {
            button2_hover = false;
            playHoverSound = false;
        }

        ImGui::SetCursorPos({buttonPosx + button_style_pos_offset.x,topPadding + 120.0f - button_style_pos_offset.y});
        ImGui::Image((void*)style_id,button_style_size,{0,1},{1,0});

        ImGui::SetCursorPos({buttonPosx,topPadding + 240.0f});

        if(ImGui::Button("Main Menu",{buttonWidth, 0})) {
            audioPlayer->playSound(our::press_button_audio.first.c_str(),false, our::press_button_audio.second);
            getApp()->changeState("main-menu");
            audioPlayer->stopSound(ost->getSoundSource());
        }
        if(ImGui::IsItemHovered() && !button3_hover) {
            audioPlayer->playSound(our::hover_button_audio.first.c_str(),false, our::hover_button_audio.second);
            button3_hover = true;
            playHoverSound = true;
        }else if(!ImGui::IsItemHovered() && !button1_hover && !button2_hover) {
            button3_hover = false;
            playHoverSound = false;
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
        static double time = glfwGetTime();
        drawMoraCount();


        drawGameplayConfigurations(glfwGetTime() - time , cameraComponent->switches , (int) cameraComponent->Divisions);
        drawTimer();
        time = glfwGetTime();

        if(gameState != PLAYING) drawEndGame();
        if(showMenu && gameState == PLAYING) drawMenu();
    }

    void destroyHUD() {
        delete mora_tex;
        delete game_over_tex;
        delete game_won_tex;
        delete paimon_icon;
        delete button_style;

        gameState = PLAYING;
        showMenu = false;
        mora_count = 0;
    }

    void onImmediateGui() override {
        ImGui::ShowDemoWindow(nullptr);
        drawHUD();
    }

    void onInitialize() override {
        // First of all, we get the scene configuration from the app config
        std::ifstream file_in(our::level_path);
        nlohmann::json config = nlohmann::json::parse(file_in, nullptr, true, true);
        file_in.close();
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
        // Then we initialize the renderer
        size = getApp()->getFrameBufferSize();

        initHUD();

        renderer.initialize(size, config["renderer"]);
        our::Events::Init(getApp() , &world);

        levelMapping.init(getApp() , &world);
        orbitalCameraControllerSystem.init(getApp());
        paimonMovement.init(getApp());
        collisionSystem.init(getApp());
        stateSystem.init(&world);


        auto audio = our::AssetLoader<std::pair<std::string, float>>::get("ost");
        game_over_audio = our::AssetLoader<std::pair<std::string, float>>::get("level_lost");
        game_won_audio = our::AssetLoader<std::pair<std::string, float>>::get("level_won");
        if (audio && !audioPlayer->isPlaying(audio->first.c_str())) {
            ost = audioPlayer->playSound(audio->first.c_str(), true, audio->second); // Play a sound with volume 0.5
        }

        for (auto k : world.getEntities()){
            auto c = k->getComponent<our::OrbitalCameraComponent>();
            if (c != nullptr){
                cameraComponent = c;
                break;
            }
        }
    }

    void onDraw(double deltaTime) override {

        if (!showMenu) time_counter += (float)deltaTime;

        // Here, we just run a bunch of systems to control the world logic
        our::Events::Update((float) deltaTime);
        paimonIdleSystem.update(&world, (float)deltaTime);


        if ((gameState == PLAYING || gameState == WON) && !showMenu) { //stop everything if the game is paused or we lost

            int gold = 0, red = 0, blue = 0;
            bool won = false;
            stateSystem.update(&world , (float) deltaTime);
            movementSystem.update(&world, (float)deltaTime);
            levelMapping.update();
            paimonMovement.update(&world , &levelMapping, (float) deltaTime , won);
            orbitalCameraControllerSystem.update(&world , (float) deltaTime);
            collisionSystem.update(&world , gold , blue , red);

            remainingTime += gold * 10;
            cameraComponent->switches += blue;
            auto temp = red;
            while (temp--)
                cameraComponent->Divisions *= 2;

            mora_count += gold + blue + red;

            if (won){ // yay
                gameState = WON;
            }
        }



        // And finally we use the renderer system to draw the scene
        renderer.render(&world);

        // Get a reference to the keyboard object
        auto& keyboard = getApp()->getKeyboard();

        if(keyboard.justPressed(GLFW_KEY_ESCAPE)){
            // If the escape  key is pressed in this frame, go to the play state
            showMenu = !showMenu;
        }

        world.deleteMarkedEntities();
    }

    void onDestroy() override {
        destroyHUD();
        renderer.destroy();
        world.clear();
        our::clearAllAssets();
    }
};