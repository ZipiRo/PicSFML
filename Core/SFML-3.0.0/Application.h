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
    static sf::Vector2u window_size;
    static std::string window_title;
    sf::Clock delta_clock;

protected:
    sf::RenderWindow window;

    sf::View canvas;
    sf::View camera;

    virtual void Start() {};
    virtual void End() {};
    virtual void Update(float delta_time) = 0;
    virtual void Event(const sf::Event &event) {};
    virtual void DrawWorld() {}; 
    virtual void DrawCanvas() {};
public:
    static sf::Color background;

    Application(sf::Vector2u size, std::string title)
    {
        window_size = size;
        window_title = title;
        
        window = sf::RenderWindow(sf::VideoMode({size.x, size.y}), title);

        canvas = window.getView();
        camera = sf::View(sf::Vector2f(0, 0), sf::Vector2f(window_size));
    }

    void Run()
    {
        Start();

        while(window.isOpen())
        {   
            Input::BeginFrame();

            Input::FetchMousePosition(
                window.mapPixelToCoords(sf::Mouse::getPosition(window), camera),
                window.mapPixelToCoords(sf::Mouse::getPosition(window), canvas)
            );

            while(auto event = window.pollEvent())
            {
                if(event->is<sf::Event::Closed>())
                    window.close();
                
                Input::FetchInputData(*event);
                Event(*event);
            }

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

    sf::Clock GetDeltaClock()
    {
        return delta_clock;
    }

    static sf::Vector2u GetWindowSize()
    {
        return window_size;
    }

    static std::string GetWindowTitle()
    {
        return window_title;
    }
};

sf::Color Application::background = sf::Color::White;
sf::Vector2u Application::window_size;
std::string Application::window_title;