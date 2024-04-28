//
// Created by HIMA on 4/26/2024.
//

#include <string>
#include "json/json.hpp"

namespace our {
    class AudioPlayer {
    public:
        AudioPlayer();
        ~AudioPlayer();

        void play(bool loop = false);

        void stop();

        [[nodiscard]] bool isPlaying() const;
        bool setVolume(int volumeLevel);

        void deserialize(const nlohmann::json& data);

    private:
        bool m_isPlaying;
        int m_volumeLevel;
        bool load(const std::string& filePath);
    };

}
