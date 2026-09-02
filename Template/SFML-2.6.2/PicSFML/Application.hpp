#pragma once

// Framework for SFML 2.6.2

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

            window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
        }

    public:
        static sf::Color background_color;

        // Note: SFML 2.x has no sf::State (windowed/fullscreen state object) --
        // fullscreen is requested by OR-ing sf::Style::Fullscreen into `style`.
        Application(sf::Vector2u size, std::string title, sf::Uint32 style = sf::Style::Default)
        {
            instance = this;
            window.create(sf::VideoMode(size.x, size.y), title, style, window_settings);

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

                sf::Event event;
                while (window.pollEvent(event))
                {
                    if (event.type == sf::Event::Closed)
                        window.close();

                    if (event.type == sf::Event::Resized)
                        window_size = sf::Vector2f(static_cast<float>(event.size.width), static_cast<float>(event.size.height));

                    Input::FetchInputData(event);
                    Events(event);
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
