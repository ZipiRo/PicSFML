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
    
        if(const auto &key = event.getIf<sf::Event::KeyPressed>())
            instance.Keyboard[key->code].pressed = true;

        if(const auto& key = event.getIf<sf::Event::KeyReleased>())
            instance.Keyboard[key->code].pressed = false;

        if(const auto &button = event.getIf<sf::Event::MouseButtonPressed>())
            instance.Mouse[button->button].pressed = true;

        if(const auto &button = event.getIf<sf::Event::MouseButtonReleased>())
            instance.Mouse[button->button].pressed = false;

        if(const auto &wheel = event.getIf<sf::Event::MouseWheelScrolled>())
            instance.mouse_wheel_delta = wheel->delta; 
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