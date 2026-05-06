#pragma once 
#include <SFML/System/Vector3.hpp>
#include <SFML/System/Time.hpp>

struct SoundSettings
{
    bool loop = false;
    bool playing_offset_enabled = false;
    float volume = 100.0f;
    float pitch = 1.0f;
    bool relative_to_listener = false;
    sf::Vector3f position = {0.f, 0.f, 0.f};
    sf::Vector3f direction = {0.f, 0.f, -1.f};
    float min_distance = 1.0f;
    float max_distance = 10000.0f;
    float attenuation = 1.0f;
    sf::Vector3f velocity = {0.f, 0.f, 0.f};
    sf::Time playing_offset = sf::Time::Zero;
};