#include "audio.hpp"
#include <iostream>
#include<windows.h>
//these two headers are already included in the <Windows.h> header
#pragma comment(lib, "Winmm.lib")
namespace our {
    AudioPlayer::AudioPlayer() : m_isPlaying(false),m_volumeLevel(100) {}

    bool AudioPlayer::load(const std::string& filePath) {
        // Open the audio file with an alias name
        if (mciSendString(
                ("open \"" + filePath + "\" type mpegvideo alias mp3").c_str(),
                NULL,
                0,
                NULL
                ) != 0) {
            std::cerr << "Failed to open audio file" << std::endl;
            return false;
        }

        return true;
    }
    void AudioPlayer::play(bool loop) {
        // Play the audio file
        std::string command = "play mp3";
        if (loop) {
            command += " repeat";
        }
        if (mciSendString(command.c_str(), NULL, 0, NULL) != 0) {
            std::cerr << "Failed to play audio file" << std::endl;
        } else {
            m_isPlaying = true;
        }
    }
    bool AudioPlayer::setVolume(int volumeLevel) {
        // Set the volume level
        std::string command = "setaudio mp3 volume to " + std::to_string(volumeLevel);
        if (mciSendString(command.c_str(), NULL, 0, NULL) != 0) {
            std::cerr << "Failed to set volume level" << std::endl;
            return false;
        } else {
            m_volumeLevel = volumeLevel;
            return true;
        }
    }

    void AudioPlayer::stop() {
        // Stop playing the audio file
        if (mciSendString("stop mp3", NULL, 0, NULL) != 0) {
            std::cerr << "Failed to stop audio file" << std::endl;
        } else {
            m_isPlaying = false;
        }
    }

    bool AudioPlayer::isPlaying() const {
        return m_isPlaying;
    }

    AudioPlayer::~AudioPlayer() {
        // Close the audio file
        mciSendString("close mp3", NULL, 0, NULL);
    }

    void AudioPlayer::deserialize(const nlohmann::json& data) {
        if (!data.is_object()) return;
        load(data.value("path" , "no path"));
        setVolume(data.value("volume" , 100));
    }
}