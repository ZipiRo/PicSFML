#pragma once

// Framework for SFML 3.0.0

#include <algorithm>
#include <SFML/Audio/Music.hpp>
#include <SoundSettings.hpp>

namespace PicSFML
{
    class MusicPlayer
    {
    private:
        sf::Music player;
        float master_volume = 100.0f;
        float local_volume = 100.0f;
    
        float ComputeVolume() const 
        {
            return std::clamp(local_volume * (master_volume / 100.0f), 0.0f, 100.0f);
        }
    
        void ApplySettings(SoundSettings &settings)
        {
            local_volume = settings.volume;

            player.setLooping(settings.loop);
            player.setVolume(ComputeVolume());
            player.setPitch(settings.pitch);
            player.setPosition(settings.position);
            player.setDirection(settings.direction);
            player.setRelativeToListener(settings.relative_to_listener);
            player.setMinDistance(settings.min_distance);
            player.setMaxDistance(settings.max_distance);
            player.setAttenuation(settings.attenuation);
        }

    public:
        static void SetMasterVolume(float volume)
        {
            auto& instance = GetInstance();
            instance.master_volume = std::clamp(volume, 0.0f, 100.0f);

            instance.player.setVolume(instance.ComputeVolume());
        }
    
        static float GetMasterVolume()
        {
            return GetInstance().master_volume;
        }
    
        static void Set(std::string music_path, SoundSettings settings = {.loop = true})
        {
            auto &instance = GetInstance();
    
            if(!instance.player.openFromFile(music_path)) return;
            instance.ApplySettings(settings);
        }
    
        static void Play(SoundSettings settings = {.loop = true})
        {
            auto &instance = GetInstance();
    
            instance.ApplySettings(settings);
            instance.player.play();
        }
    
        static void Stop()
        {
            GetInstance().player.stop();
        }

        static void ChangeSettings(SoundSettings settings = {.loop = true})
        {
            GetInstance().ApplySettings(settings);
        }
    
        static MusicPlayer& GetInstance()
        {
            static MusicPlayer instance;
            return instance;
        }
    };
}