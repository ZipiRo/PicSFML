#pragma once

// Framework for SFML 2.6.2

#include <unordered_map>
#include <filesystem>
#include <string>
#include <utility>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Shader.hpp>

#ifdef AUDIO
#include <SFML/Audio/SoundBuffer.hpp>
#endif

namespace PicSFML
{
    template <typename T>
    class Resource
    {
    private:
        std::unordered_map<std::string, T> data;

    public:
        Resource() = default;

        void Insert(std::string name, T value)
        {
            data.insert_or_assign(std::move(name), std::move(value));
        }

        bool Exists(const std::string &name) const
        {
            return data.find(name) != data.end();
        }

        T &GetOrCreate(const std::string &name)
        {
            return data[name];
        }

        void Remove(const std::string &name)
        {
            data.erase(name);
        }

        T &Get(const std::string &name)
        {
            return data.at(name);
        }

        const T &Get(const std::string &name) const
        {
            return data.at(name);
        }
    };

    class ResourceManager
    {
    private:
        ResourceManager() = default;

    public:
        static inline Resource<sf::Texture> Textures;
        static inline Resource<sf::Font> Fonts;
        static inline Resource<sf::Shader> Shaders;

#ifdef AUDIO
        static inline Resource<std::string> Music;
        static inline Resource<sf::SoundBuffer> Sounds;
#endif

        static bool LoadTexture(std::string name, std::string path)
        {
            sf::Texture texture;
            if (!texture.loadFromFile(path))
                return false;

            Textures.Insert(std::move(name), std::move(texture));
            return true;
        }

        static bool LoadFont(std::string name, std::string path)
        {
            sf::Font font;
            if (!font.loadFromFile(path))
                return false;

            Fonts.Insert(std::move(name), std::move(font));
            return true;
        }

        static bool LoadShader(std::string name, std::string vertex_path, std::string fragment_path)
        {
            sf::Shader &shader = Shaders.GetOrCreate(name);

            if (!vertex_path.empty() && !fragment_path.empty())
            {
                if (!shader.loadFromFile(vertex_path, fragment_path))
                {
                    Shaders.Remove(name);
                    return false;
                }
            }
            else if (!vertex_path.empty())
            {
                if (!shader.loadFromFile(vertex_path, sf::Shader::Type::Vertex))
                {
                    Shaders.Remove(name);
                    return false;
                }
            }
            else if (!fragment_path.empty())
            {
                if (!shader.loadFromFile(fragment_path, sf::Shader::Type::Fragment))
                {
                    Shaders.Remove(name);
                    return false;
                }
            }
            else
            {
                Shaders.Remove(name);
                return false;
            }

            return true;
        }

#ifdef AUDIO
        static bool LoadSound(std::string name, std::string path)
        {
            sf::SoundBuffer sound;
            if (!sound.loadFromFile(path))
                return false;

            Sounds.Insert(std::move(name), std::move(sound));
            return true;
        }

        static bool LoadMusic(std::string name, std::string path)
        {
            if (!std::filesystem::exists(path))
                return false;

            Music.Insert(std::move(name), std::move(path));
            return true;
        }
#endif

        static void LoadFromResourcesDirectory()
        {
            if (!std::filesystem::exists("Resources"))
                return;

#ifdef AUDIO
            if (std::filesystem::exists("Resources\\Audio"))
            {
                for (const auto &entry : std::filesystem::recursive_directory_iterator("Resources\\Audio"))
                {
                    if (!entry.is_regular_file())
                        continue;
                    std::filesystem::path entry_path = entry.path();
                    std::string extension = entry_path.extension().string();

                    if (extension == ".wav" || extension == ".ogg" || extension == ".flac" ||
                        extension == ".aiff" || extension == ".au" || extension == ".raw")
                    {
                        std::string name = entry_path.filename().string();
                        LoadSound(std::move(name), entry_path.string());
                    }
                }
            }

            if (std::filesystem::exists("Resources\\Music"))
            {
                for (const auto &entry : std::filesystem::recursive_directory_iterator("Resources\\Music"))
                {
                    if (!entry.is_regular_file())
                        continue;
                    std::filesystem::path entry_path = entry.path();
                    std::string extension = entry_path.extension().string();

                    if (extension == ".mp3" || extension == ".wav" || extension == ".ogg" ||
                        extension == ".flac" || extension == ".aiff" || extension == ".au" ||
                        extension == ".raw")
                    {
                        std::string name = entry_path.filename().string();
                        LoadMusic(std::move(name), entry_path.string());
                    }
                }
            }
#endif

            if (std::filesystem::exists("Resources\\Fonts"))
            {
                for (const auto &entry : std::filesystem::recursive_directory_iterator("Resources\\Fonts"))
                {
                    if (!entry.is_regular_file())
                        continue;
                    std::filesystem::path entry_path = entry.path();
                    std::string extension = entry_path.extension().string();

                    if (extension == ".ttf" || extension == ".otf")
                    {
                        std::string name = entry_path.filename().string();
                        LoadFont(std::move(name), entry_path.string());
                    }
                }
            }

            if (std::filesystem::exists("Resources\\Shaders"))
            {
                for (const auto &entry : std::filesystem::recursive_directory_iterator("Resources\\Shaders"))
                {
                    if (!entry.is_regular_file())
                        continue;
                    std::filesystem::path entry_path = entry.path();
                    std::string extension = entry_path.extension().string();

                    if (extension != ".frag")
                        continue;

                    std::string name = entry_path.stem().string();
                    std::filesystem::path vert_path = entry_path.parent_path() / (name + ".vert");
                    std::string fragment_path = entry_path.string();
                    std::string vertex_path;

                    if (std::filesystem::exists(vert_path))
                        vertex_path = vert_path.string();

                    LoadShader(std::move(name), std::move(vertex_path), std::move(fragment_path));
                }
            }

            if (std::filesystem::exists("Resources\\Textures"))
            {
                for (const auto &entry : std::filesystem::recursive_directory_iterator("Resources\\Textures"))
                {
                    if (!entry.is_regular_file())
                        continue;
                    std::filesystem::path entry_path = entry.path();
                    std::string extension = entry_path.extension().string();

                    if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" ||
                        extension == ".bmp" || extension == ".tga" || extension == ".gif" ||
                        extension == ".pic" || extension == ".hdr" || extension == ".psd")
                    {
                        std::string name = entry_path.filename().string();
                        LoadTexture(std::move(name), entry_path.string());
                    }
                }
            }
        }
    };
}
