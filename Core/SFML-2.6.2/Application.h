#pragma once
#include <SFML/Graphics.hpp>
#include <ResourceManager.h>
#include <Input.h>

#ifdef AUDIO
#include <SFML/Audio.hpp>
#include <SoundPlayer.h>
#include <MusicPlayer.h>
#endif

#ifdef NETWORK
#include <SFML/Network.hpp>
#endif

#include <Utils.h>

class Application
{
private:
    static Application* instance;

    sf::RenderWindow window;
    sf::Vector2f window_size;
    std::string window_title;

    sf::Clock delta_clock;

    sf::View canvas;
    sf::View camera;

protected:

    sf::ContextSettings window_settings;

    virtual void Start() {};
    virtual void End() {};
    virtual void Update(float delta_time) {};
    virtual void Events(const sf::Event &event) {};
    virtual void DrawWorld() {}; 
    virtual void DrawCanvas() {};

    void SetWindowIcon()
    {
        if(!std::filesystem::exists("icon.png")) return;

        sf::Image icon;
        if(!icon.loadFromFile("icon.png")) return;

        window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    }

public:
    static sf::Color background;

    Application(sf::Vector2u size, std::string title, sf::Uint32 style = sf::Style::Default)
    {
        instance = this;
        window.create(sf::VideoMode({size.x, size.y}), title, style, window_settings);
        
        SetWindowIcon();

        window_size = sf::Vector2f(size);
        window_title = title;
        canvas = window.getView();
        camera = sf::View(sf::Vector2f(0, 0), sf::Vector2f(size));
    }

    void Run()
    {
        Start();

        while(window.isOpen())
        {   
            Input::BeginFrame();

            sf::Event event;
            while(window.pollEvent(event))
            {
                if(event.type == sf::Event::Closed)
                    window.close();

                if(event.type == sf::Event::Resized)
                {
                    window_size = sf::Vector2f(window.getSize());

                    canvas.setSize(window_size);
                    camera.setSize(window_size);
                }
                
                Input::FetchInputData(event);
                Events(event);
            }

            Input::FetchMousePosition(
                window.mapPixelToCoords(sf::Mouse::getPosition(window), camera),
                window.mapPixelToCoords(sf::Mouse::getPosition(window), canvas)
            );

            float delta_time = delta_clock.restart().asSeconds();

            Update(delta_time);

            window.clear(background);

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

    static sf::Clock GetDeltaClock()
    {
        return instance->delta_clock;
    }

    static sf::View GetCamera()
    {
        return instance->camera;
    }

    static sf::View GetCanvas()
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

sf::Color Application::background = sf::Color::White;
Application* Application::instance = nullptr;