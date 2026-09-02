#pragma once

// Framework for SFML 3.0.0

#include <vector>
#include <algorithm>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SoundSettings.hpp>

namespace PicSFML
{
    static const sf::SoundBuffer dummy_sound_buffer;

    struct PlayerEntity
    {
        std::string name;
        sf::Sound sound_player;
        float local_volume = 100.0f;
    };

    class SoundPlayer
    {
    private:
        std::vector<PlayerEntity> player_entities;
        float master_volume = 100.0f;

        SoundPlayer()
        {
            player_entities.assign(32, PlayerEntity{"", sf::Sound(dummy_sound_buffer)});
        }

        float ComputeVolume(float local) const 
        {
            return std::clamp(local * (master_volume / 100.0f), 0.0f, 100.0f);
        }
        
        void ApplySettings(PlayerEntity& entity, const SoundSettings& settings)
        {
            entity.local_volume = settings.volume;

            entity.sound_player.setLooping(settings.loop);
            entity.sound_player.setVolume(ComputeVolume(entity.local_volume));
            entity.sound_player.setPitch(settings.pitch);
            entity.sound_player.setPosition(settings.position);
            entity.sound_player.setDirection(settings.direction);
            entity.sound_player.setRelativeToListener(settings.relative_to_listener);
            entity.sound_player.setMinDistance(settings.min_distance);
            entity.sound_player.setMaxDistance(settings.max_distance);
            entity.sound_player.setAttenuation(settings.attenuation);
            entity.sound_player.setVelocity(settings.velocity);

            if (settings.playing_offset_enabled)
                entity.sound_player.setPlayingOffset(settings.playing_offset);
        }

    public:
        static void SetMaxPlayers(int amount)
        {
            if(amount < 1) return; 
            GetInstance().player_entities.assign(amount, PlayerEntity{"", sf::Sound(dummy_sound_buffer)});
        }

        static void SetMasterVolume(float volume)
        {
            auto& instance = GetInstance();

            instance.master_volume = std::clamp(volume, 0.0f, 100.0f);

            for (auto& entity : instance.player_entities)
                entity.sound_player.setVolume(instance.ComputeVolume(entity.local_volume));
        }

        static float GetMasterVolume()
        {
            return GetInstance().master_volume;
        }
        
        static void Play(const sf::SoundBuffer& buffer, const std::string &sound_name, SoundSettings settings = {})
        {
            auto& instance = GetInstance();

            for (auto& entity : instance.player_entities)
            {
                if (entity.sound_player.getStatus() != sf::Sound::Status::Stopped) continue;

                entity.name = sound_name;

                entity.sound_player.setBuffer(buffer);
                instance.ApplySettings(entity, settings);

                entity.sound_player.play();
                return;
            }
        }

        static void Resume(const std::string& sound_name)
        {
            auto& instance = GetInstance();

            for(auto& entity : instance.player_entities)
            {
                if(entity.name == sound_name)
                {
                    entity.sound_player.play();
                    return;
                }
            }
        }

        static void Stop(const std::string& sound_name)
        {
            auto& instance = GetInstance();

            for(auto& entity : instance.player_entities)
            {
                if(entity.name == sound_name)
                {
                    entity.sound_player.stop();
                    return;
                }
            }
        }

        static void Pause(const std::string& sound_name)
        {
            auto& instance = GetInstance();

            for(auto& entity : instance.player_entities)
            {
                if(entity.name == sound_name)
                {
                    entity.sound_player.pause();
                    return;
                }
            }
        }

        static SoundPlayer& GetInstance()
        {
            static SoundPlayer instance;
            return instance;
        }
    };
}