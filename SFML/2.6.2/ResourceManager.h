#pragma once
#include <unordered_map>
#include <filesystem>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>

#ifdef AUDIO
#include <SFML/Audio/SoundBuffer.hpp>
#endif

template <typename T>
class Resource
{
private:
    std::unordered_map<std::string, T> data;

public:
    Resource() {}

    void Insert(std::string name, T &value)
    {   
        data[name] = value;
    }

    bool Exists(std::string name)
    {
        return data.find(name) != data.end();
    }

    T &Get(std::string name)
    {
        return data.at(name);
    }
};

class ResourceManager
{
private:
    ResourceManager() {}

public:
    static Resource<sf::Texture> Textures;
    static Resource<sf::Font> Fonts;

    static bool LoadTexture(std::string name, std::string path)
    {
        sf::Texture texture;
        if(!texture.loadFromFile(path))
            return false;

        Textures.Insert(name, texture);
        return true;
    }

    static bool LoadFont(std::string name, std::string path)
    {
        sf::Font font;

        if(!font.loadFromFile(path))
            return false;

        Fonts.Insert(name, font);
        return true;
    }
    
#ifdef AUDIO
    static Resource<std::string> Music;
    static Resource<sf::SoundBuffer> Sounds;

    static bool LoadSound(std::string name, std::string path)
    {
        sf::SoundBuffer sound;
        if(!sound.loadFromFile(path))
            return false;

        Sounds.Insert(name, sound);
        return true;
    }

    static bool LoadMusic(std::string name, std::string path)
    {
        if(!std::filesystem::exists(path))
            return false;
            
        Music.Insert(name, path);
        return true;
    }
#endif
};

Resource<sf::Texture> ResourceManager::Textures;
Resource<sf::Font> ResourceManager::Fonts;

#ifdef AUDIO
Resource<sf::SoundBuffer> ResourceManager::Sounds;
Resource<std::string> ResourceManager::Music;
#endif