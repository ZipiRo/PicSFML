#pragma once
#include <vector>
#include <algorithm>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SoundSettings.h>

static const sf::SoundBuffer dummy_sound_buffer;

class SoundPlayer
{
private:
    std::vector<sf::Sound> players;
    float master_volume = 100.0f;

    SoundPlayer()
    {
        players.assign(32, sf::Sound(dummy_sound_buffer));
    }

    float ComputeVolume(float local) const 
    {
        return std::clamp(local * (master_volume / 100.0f), 0.0f, 100.0f);
    }

public:
    static void SetMaxPlayers(int amount)
    {
        auto& instance = GetInstance();
        instance.players.assign(amount, sf::Sound(dummy_sound_buffer));
    }

    static void SetMasterVolume(float volume)
    {
        auto& instance = GetInstance();
        instance.master_volume = volume;
    }

    static int GetMasterVolume()
    {
        auto& instance = GetInstance();
        return instance.master_voulume;
    }

    static void Play(const sf::SoundBuffer& buffer, SoundSettings settings = {})
    {
        auto& instance = GetInstance();

        for (auto& player : instance.players)
        {
            if (player.getStatus() != sf::Sound::Status::Stopped) continue;

            player.setBuffer(buffer);
            player.setLoop(settings.loop);
            player.setVolume(instance.ComputeVolume(settings.volume));
            player.setPitch(settings.pitch);
            player.setPosition(settings.position);
            player.setRelativeToListener(settings.relative_to_listener);
            player.setAttenuation(settings.attenuation);

            if (settings.playing_offset_enabled)
                player.setPlayingOffset(settings.playing_offset);

            player.play();
            return;
        }
    }

    static SoundPlayer& GetInstance()
    {
        static SoundPlayer instance;
        return instance;
    }
};