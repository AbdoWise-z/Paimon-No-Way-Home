
#pragma once

#include <application.hpp>
#include <texture/texture2d.hpp>
#include <texture/texture-utils.hpp>
#include <iomanip>
#include <filesystem>

#include "../globals.h"

class MainMenuState : public our::State{

    //textures
    std::vector<our::Texture2D*> main_menu_tex;
    our::Texture2D* main_menu_logo;
    our::Texture2D* button_style;
    //audio
    our::AudioPlayer* audioPlayer = our::AudioPlayer::getInstance();
    //background
    int main_menu_index = 0;
    float accumaltedTime = 0;

    void onInitialize() override {
        our::curr_level = 0;

        int i = 0;
        while (true) {
            std::stringstream ss;
            ss << "assets/textures/main_menu/frame_" << std::setw(0) << std::setfill('0') << i << "_delay-0.1s.png";
            // std::cout << "Loading: " << ss.str() << std::endl;

            if ( ! std::filesystem::exists(ss.str()) ) break;
            main_menu_tex.push_back(our::texture_utils::loadImage(ss.str()));
            i++;
        }

        main_menu_logo = our::texture_utils::loadImage("assets/textures/main_menu/main_menu.png");
        button_style = our::texture_utils::loadImage("assets/textures/button_style.png");
        our::ost_path = "assets/sounds/osts/Lovers_Oath.mp3";
        if(!audioPlayer->isPlaying(our::ost_path)) {
            our::ost = audioPlayer->playSound(our::ost_path,true,0.2f);
        }
    }

    void onImmediateGui() override {
        static bool playHoverSound = false;
        static bool start_hover = false;
        static bool exit_hover = false;
        ImGui::Begin("main_menu" , nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollWithMouse
            | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar
            | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);

        ImGui::SetWindowFontScale(3);
        ImGui::SetWindowSize({1280,720});
        ImGui::SetWindowPos({0,0});

        GLuint id = main_menu_tex[main_menu_index % main_menu_tex.size()]->getOpenGLName();
        ImGui::SetCursorPos({0,0});
        ImGui::Image((void*)id,{1280,720},{0,1},{1,0});

        GLuint logo_id = main_menu_logo->getOpenGLName();
        ImGui::SetCursorPos({0,0});
        ImGui::Image((void*)logo_id,{1280,720},{0,1},{1,0});

        ImGui::SetCursorPos({1280/2 - 140/2,500});
        if(ImGui::Button("Start",{140,0})) {
            audioPlayer->playSound(our::press_button_audio.first.c_str(),false, our::press_button_audio.second);
            getApp()->changeState("level-menu");
        }
        if(ImGui::IsItemHovered() && !playHoverSound) {
            audioPlayer->playSound(our::hover_button_audio.first.c_str(),false, our::hover_button_audio.second);
            playHoverSound = true;
            start_hover = true;
        }else if(!ImGui::IsItemHovered() && !exit_hover){
            playHoverSound = false;
            start_hover = false;
        }
        GLuint style_id = button_style->getOpenGLName();

        ImGui::SetCursorPos({1280/2 - 140/2 + 100,500 - 16});
        ImGui::Image((void*)style_id,{50.0f,50.0f},{0,1},{1,0});

        ImGui::SetCursorPos({1280/2 - 140/2,500 + 80});
        if(ImGui::Button("Exit",{140,0})) {
            audioPlayer->playSound(our::press_button_audio.first.c_str(),false, our::press_button_audio.second);
            for(auto i : main_menu_tex) {
                delete i;
            }
            getApp()->close();
        }
        if(ImGui::IsItemHovered() && !playHoverSound) {
            audioPlayer->playSound(our::hover_button_audio.first.c_str(),false, our::hover_button_audio.second);
            playHoverSound = true;
            exit_hover = true;
        }else if(!ImGui::IsItemHovered() && !start_hover){
            playHoverSound = false;
            exit_hover = false;
        }

        ImGui::SetCursorPos({1280/2 - 140/2 + 100,500 + 80 - 16});
        ImGui::Image((void*)style_id,{50.0f,50.0f},{0,1},{1,0});

        ImGui::End();
    }

    void onDraw(double deltaTime) override {
        accumaltedTime += deltaTime;
        if(accumaltedTime > 0.1f) {
            accumaltedTime = 0.0f;
            main_menu_index++;
            main_menu_index %= 100;
        }
    }

    void onDestroy() override {

        delete main_menu_logo;
        delete button_style;
        //audioPlayer->stopSound(our::ost->getSoundSource());
    }
};


