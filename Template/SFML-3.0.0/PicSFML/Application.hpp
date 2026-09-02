#pragma once

// Framework for SFML 3.0.0

#include <SFML/Graphics.hpp>
#include <ResourceManager.hpp>
#include <Input.hpp>

#ifdef AUDIO
#include <SFML/Audio.hpp>
#include <SoundPlayer.hpp>
#include <MusicPlayer.hpp>
#endif

#ifdef NETWORK
#include <SFML/Network.hpp>
#endif

namespace PicSFML
{
    class Application
    {
    private:
        static Application *instance;
        static float delta_time;

        sf::RenderWindow window;
        sf::Vector2f window_size;
        std::string window_title;

        sf::View canvas;
        sf::View camera;

    protected:
        sf::ContextSettings window_settings;

        virtual void Start() {};
        virtual void Update() {};
        virtual void Events(const sf::Event &event) {};
        virtual void DrawWorld() {};
        virtual void DrawCanvas() {};
        virtual void End() {};

        void SetWindowIcon()
        {
            if (!std::filesystem::exists("icon.png"))
                return;

            sf::Image icon;
            if (!icon.loadFromFile("icon.png"))
                return;

            window.setIcon(icon);
        }

    public:
        static sf::Color background_color;

        Application(sf::Vector2u size, std::string title, uint32_t style = sf::Style::Default, sf::State state = sf::State::Windowed)
        {
            instance = this;
            window = sf::RenderWindow(sf::VideoMode({size.x, size.y}), title, style, state, window_settings);

            SetWindowIcon();

            window_size = sf::Vector2f(size);
            window_title = title;
            canvas = window.getView();
            camera = sf::View(sf::Vector2f(0, 0), sf::Vector2f(size));
        }

        void Run()
        {
            sf::Clock delta_clock;
            
            ResourceManager::LoadFromResourcesDirectory();

            Start();

            while (window.isOpen())
            {
                Input::BeginFrame();

                while (auto event = window.pollEvent())
                {
                    if (event->is<sf::Event::Closed>())
                        window.close();

                    if (event->is<sf::Event::Resized>())
                        window_size = sf::Vector2f(window.getSize());

                    Input::FetchInputData(*event);
                    Events(*event);
                }

                Input::FetchMousePosition(
                    window.mapPixelToCoords(sf::Mouse::getPosition(window), camera),
                    window.mapPixelToCoords(sf::Mouse::getPosition(window), canvas));

                delta_time = delta_clock.restart().asSeconds();

                Update();

                window.clear(background_color);

                window.setView(camera);
                DrawWorld();

                window.setView(canvas);
                DrawCanvas();

                window.display();
            }

            End();
        }

        static void Close()
        {
            instance->window.close();
        }

        static float DeltaTime()
        {
            return instance->delta_time;
        }

        static sf::View &GetCamera()
        {
            return instance->camera;
        }

        static sf::View &GetCanvas()
        {
            return instance->canvas;
        }

        static std::string GetWindowTitle()
        {
            return instance->window_title;
        }

        static sf::Vector2f GetWindowSize()
        {
            return instance->window_size;
        }

        static sf::RenderWindow &GetWindow()
        {
            return instance->window;
        }
    };

    sf::Color Application::background_color = sf::Color::White;
    Application *Application::instance = nullptr;
    float Application::delta_time;
}