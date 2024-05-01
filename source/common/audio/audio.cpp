#include "audio.hpp"
#include <iostream>
#include<windows.h>


// Define the static member variable
irrklang::ISoundEngine* our::AudioPlayer::SoundEngine = nullptr;
our::AudioPlayer* our::AudioPlayer::instance = nullptr;

// Constructor
our::AudioPlayer::AudioPlayer() {
    // Initialize SoundEngine if it's not already initialized
    if (!SoundEngine) {
        SoundEngine = irrklang::createIrrKlangDevice();
        // If creation fails, handle the error appropriately
        if (!SoundEngine) {
            // Handle initialization failure
        }
    }
}

// Destructor
our::AudioPlayer::~AudioPlayer() {
    // Destroy SoundEngine only if it's initialized
    if (SoundEngine) {
        SoundEngine->drop(); // Release resources
        SoundEngine = nullptr; // Reset the pointer
    }
}

ISound* our::AudioPlayer::playSound(const char* soundFile, bool looped, float volume) {
    if (SoundEngine) {
        // Play the sound with the specified volume
        auto audio = SoundEngine->play2D(soundFile, looped, false, true, ESM_STREAMING, false);
        audio->setVolume(volume);
        audio->setPlaybackSpeed(1);
        return audio;
    }
}
bool our::AudioPlayer::isPlaying(const char* soundFile) {
        // Play the sound with the specified volume
        return SoundEngine->isCurrentlyPlaying(soundFile);
}
void our::AudioPlayer::stopAllSounds() {
    SoundEngine->stopAllSounds();
}
void our::AudioPlayer::stopSound(ISoundSource* src) {
    SoundEngine->stopAllSoundsOfSoundSource(src);
}
// Function to get the singleton instance
our::AudioPlayer* our::AudioPlayer::getInstance() {
    if (!instance) {
        instance = new AudioPlayer();
    }
    return instance;
}