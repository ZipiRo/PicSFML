#pragma once
#include <algorithm>
#include <SFML/Audio/Music.hpp>
#include <SoundSettings.h>

class MusicPlayer
{
private:
    sf::Music player;
    float master_volume = 100.0f;

    float ComputeVolume(float local) const 
    {
        return std::clamp(local * (master_volume / 100.0f), 0.0f, 100.0f);
    }

public:
    static void Set(std::string music_path, SoundSettings settings = {.loop = true})
    {
        auto &instance = GetInstance();

        sf::Music &player = instance.player;

        player.openFromFile(music_path);
        player.setLoop(settings.loop);
        player.setVolume(instance.ComputeVolume(settings.volume));
        player.setPitch(settings.pitch);
        player.setPosition(settings.position);
        player.setRelativeToListener(settings.relative_to_listener);
        player.setAttenuation(settings.attenuation);
    }

    static void Play(SoundSettings settings = {.loop = true})
    {
        auto &instance = GetInstance();
        sf::Music &player = instance.player;

        player.setLoop(settings.loop);
        player.setVolume(instance.ComputeVolume(settings.volume));
        player.setPitch(settings.pitch);
        player.setPosition(settings.position);
        player.setRelativeToListener(settings.relative_to_listener);
        player.setAttenuation(settings.attenuation);
        player.play();
    }

    static void Stop()
    {
        auto &instance = GetInstance();
        instance.player.stop();
    }

    static MusicPlayer& GetInstance()
    {
        static MusicPlayer instance;
        return instance;
    }
};