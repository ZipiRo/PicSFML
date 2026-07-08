#pragma once
#include <unordered_map>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>

class Input
{
private:
    Input() {}

    struct KeyState
    {
        bool pressed = false;
        bool was_pressed = false;
    };

    struct ButtonState
    {
        bool pressed = false;
        bool was_pressed = false;
    };

    std::unordered_map<sf::Keyboard::Key, KeyState> Keyboard;
    std::unordered_map<sf::Mouse::Button, ButtonState> Mouse;
    float mouse_wheel_delta;
    sf::Vector2f mouse_world_position;
    sf::Vector2f mouse_canvas_poition;

public:
    static void BeginFrame()
    {
        auto &instance = GetInstance();

        for(auto &[key, state] : instance.Keyboard)
            state.was_pressed = state.pressed;
    
        for(auto &[button, state] : instance.Mouse)
            state.was_pressed = state.pressed;

        instance.mouse_wheel_delta = 0.0f;
    }

    static void FetchInputData(const sf::Event &event)
    {
        auto &instance = GetInstance();
    
        switch (event.type)
        {
            case sf::Event::KeyPressed:
                instance.Keyboard[event.key.code].pressed = true;
                break;

            case sf::Event::KeyReleased:
                instance.Keyboard[event.key.code].pressed = false;
                break;

            case sf::Event::MouseButtonPressed:
                instance.Mouse[event.mouseButton.button].pressed = true;
                break;

            case sf::Event::MouseButtonReleased:
                instance.Mouse[event.mouseButton.button].pressed = false;
                break;

            case sf::Event::MouseWheelScrolled:
                instance.mouse_wheel_delta = event.mouseWheelScroll.delta;
                break;

            default:
                break;
        }
    }

    static void FetchMousePosition(sf::Vector2f mouse_world, sf::Vector2f mouse_canvas)
    {
        auto &instance = GetInstance();

        instance.mouse_world_position = mouse_world;
        instance.mouse_canvas_poition = mouse_canvas;
    }

    static bool IsKeyDown(sf::Keyboard::Key key)
    {
        auto &instance = GetInstance();
        auto it = instance.Keyboard.find(key);

        if(it != instance.Keyboard.end())
        {
            const auto &state = it->second;
            return state.pressed && !state.was_pressed;
        }

        return false;
    }
    
    static bool IsKeyUp(sf::Keyboard::Key key)
    {
        auto &instance = GetInstance();
        auto it = instance.Keyboard.find(key);

        if(it != instance.Keyboard.end())
        {
            const auto &state = it->second;
            return !state.pressed && state.was_pressed;
        }
        
        return false;
    }
    
    static bool IsKey(sf::Keyboard::Key key)
    {
        auto &instance = GetInstance();
        auto it = instance.Keyboard.find(key);

        if(it != instance.Keyboard.end())
        {
            const auto &state = it->second;
            return state.pressed && state.was_pressed;
        }
        
        return false;
    }

    static bool IsMouseButtonDown(sf::Mouse::Button button)
    {
        auto &instance = GetInstance();
        auto it = instance.Mouse.find(button);

        if(it != instance.Mouse.end())
        {
            const auto &state = it->second;
            return state.pressed && !state.was_pressed;
        }
        
        return false;
    }

    static bool IsMouseButtonUp(sf::Mouse::Button button)
    {
        auto &instance = GetInstance();
        auto it = instance.Mouse.find(button);

        if(it != instance.Mouse.end())
        {
            const auto &state = it->second;
            return !state.pressed && state.was_pressed;
        }
        
        return false;
    }

    static bool IsMouseButton(sf::Mouse::Button button)
    {
        auto &instance = GetInstance();
        auto it = instance.Mouse.find(button);

        if(it != instance.Mouse.end())
        {
            const auto &state = it->second;
            return state.pressed && state.was_pressed;
        }
        
        return false;
    }

    static float MouseWheelDelta()
    {
        auto &instance = GetInstance();
        return instance.mouse_wheel_delta;
    }

    static sf::Vector2f GetMouseWorldPosition()
    {
        auto &instance = GetInstance();
        return instance.mouse_world_position;
    }

    static sf::Vector2f GetMouseCanvasPosition()
    {
        auto &instance = GetInstance();
        return instance.mouse_canvas_poition;
    }

    static Input &GetInstance()
    {
        static Input instance;
        return instance;
    }
};