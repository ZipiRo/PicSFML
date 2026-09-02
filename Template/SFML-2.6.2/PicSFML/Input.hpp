#pragma once

// Framework for SFML 2.6.2

#include <unordered_map>
#include <vector>
#include <optional>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>

namespace PicSFML
{
    enum class GamepadButton
    {
        X,
        A,
        B,
        Y,
        LeftShoulder,
        RightShoulder,
        LeftTrigger,
        RightTrigger,
        Back,
        Start,
        LeftStick,
        RightStick
    };

    enum class GamepadAxis
    {
        LeftX,
        LeftY,
        RightX,
        RightY,
        LeftTrigger,
        RightTrigger,
        PovX,
        PovY
    };

    class Input
    {
    private:
        Input() 
        {
            Gamepad.assign(8, GamepadState());
            
            for(int i = 0; i < 8; i++)
                Gamepad[i].connected = sf::Joystick::isConnected(i);
        }

        struct ButtonState
        {
            bool pressed = false;
            bool was_pressed = false;
        };

        struct MouseState
        {
            float wheel_delta = 0.0f;
            sf::Vector2f world_position;
            sf::Vector2f canvas_position;
            std::unordered_map<sf::Mouse::Button, ButtonState> buttons;
        };

        struct GamepadState  
        {
            bool connected = false;
            std::unordered_map<GamepadButton, ButtonState> buttons;
            std::unordered_map<GamepadAxis, float> joysticks;

            void Reset()
            {
                buttons.clear();
                joysticks.clear();
            }
        };

        std::unordered_map<sf::Keyboard::Key, ButtonState> Keyboard;
        std::vector<GamepadState> Gamepad;
        MouseState Mouse;

    public:
        static void BeginFrame()
        {
            auto &instance = GetInstance();

            for(auto &[key, state] : instance.Keyboard)
                state.was_pressed = state.pressed;
            
            for(auto &gamepad : instance.Gamepad)
            {
                if(!gamepad.connected) 
                {
                    gamepad.Reset();
                    continue;
                }

                for(auto &[button, state] : gamepad.buttons)
                    state.was_pressed = state.pressed;
            }

            for(auto &[button, state] : instance.Mouse.buttons)
                state.was_pressed = state.pressed;

            instance.Mouse.wheel_delta = 0.0f;
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
                    instance.Mouse.buttons[event.mouseButton.button].pressed = true;
                    break;

                case sf::Event::MouseButtonReleased:
                    instance.Mouse.buttons[event.mouseButton.button].pressed = false;
                    break;

                case sf::Event::MouseWheelScrolled:
                    instance.Mouse.wheel_delta = event.mouseWheelScroll.delta;
                    break;

                case sf::Event::JoystickConnected:
                    instance.Gamepad[event.joystickConnect.joystickId].connected = true;
                    break;

                case sf::Event::JoystickDisconnected:
                    instance.Gamepad[event.joystickConnect.joystickId].connected = false;
                    break;

                case sf::Event::JoystickButtonPressed:
                    instance.Gamepad[event.joystickButton.joystickId].buttons[GamepadButton(event.joystickButton.button)].pressed = true;
                    break;

                case sf::Event::JoystickButtonReleased:
                    instance.Gamepad[event.joystickButton.joystickId].buttons[GamepadButton(event.joystickButton.button)].pressed = false;
                    break;

                case sf::Event::JoystickMoved:
                    instance.Gamepad[event.joystickMove.joystickId].joysticks[GamepadAxis(event.joystickMove.axis)] = event.joystickMove.position / 100.0f;
                    break;

                default:
                    break;
            }
        }

        static void FetchMousePosition(sf::Vector2f mouse_world, sf::Vector2f mouse_canvas)
        {
            auto &instance = GetInstance();

            instance.Mouse.world_position = mouse_world;
            instance.Mouse.canvas_position = mouse_canvas;
        }

        static bool IsKeyPressed(sf::Keyboard::Key key)
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
        
        static bool IsKeyReleased(sf::Keyboard::Key key)
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
        
        static bool IsKeyHeld(sf::Keyboard::Key key)
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

        static bool IsMouseButtonPressed(sf::Mouse::Button button)
        {
            auto &instance = GetInstance();
            auto it = instance.Mouse.buttons.find(button);

            if(it != instance.Mouse.buttons.end())
            {
                const auto &state = it->second;
                return state.pressed && !state.was_pressed;
            }
            
            return false;
        }

        static bool IsMouseButtonReleased(sf::Mouse::Button button)
        {
            auto &instance = GetInstance();
            auto it = instance.Mouse.buttons.find(button);

            if(it != instance.Mouse.buttons.end())
            {
                const auto &state = it->second;
                return !state.pressed && state.was_pressed;
            }
            
            return false;
        }

        static bool IsMouseButtonHeld(sf::Mouse::Button button)
        {
            auto &instance = GetInstance();
            auto it = instance.Mouse.buttons.find(button);

            if(it != instance.Mouse.buttons.end())
            {
                const auto &state = it->second;
                return state.pressed && state.was_pressed;
            }
            
            return false;
        }

        static bool IsGamepadButtonPressed(GamepadButton button, int id = 0)
        {
            auto &instance = GetInstance();
            
            if(!instance.Gamepad[id].connected) return false;

            auto it = instance.Gamepad[id].buttons.find(button);

            if(it != instance.Gamepad[id].buttons.end())
            {
                const auto &state = it->second;
                return state.pressed && !state.was_pressed;
            }

            return false;
        }

        static bool IsGamepadButtonReleased(GamepadButton button, int id = 0)
        {
            auto &instance = GetInstance();
            
            if(!instance.Gamepad[id].connected) return false;

            auto it = instance.Gamepad[id].buttons.find(button);

            if(it != instance.Gamepad[id].buttons.end())
            {
                const auto &state = it->second;
                return !state.pressed && state.was_pressed;
            }

            return false;

        }
        
        static bool IsGamepadButtonHeld(GamepadButton button, int id = 0)
        {
            auto &instance = GetInstance();
            
            if(!instance.Gamepad[id].connected) return false;

            auto it = instance.Gamepad[id].buttons.find(button);

            if(it != instance.Gamepad[id].buttons.end())
            {
                const auto &state = it->second;
                return state.pressed && state.was_pressed;
            }

            return false;
        }

        static bool IsGamepadConnected(int id)
        {
            return GetInstance().Gamepad[id].connected;
        }

        static float GetGamepadAxis(GamepadAxis axis, int id = 0)
        {
            auto &instance = GetInstance();
            
            if(!instance.Gamepad[id].connected) return 0.0f;
            
            auto it = instance.Gamepad[id].joysticks.find(axis);

            if(it != instance.Gamepad[id].joysticks.end())
            {
                const auto &position = it->second;

                if(std::abs(position) < 0.15f) return 0.0f;

                return it->second;
            }

            return 0.0f;
        }

        static float MouseWheelDelta()
        {
            auto &instance = GetInstance();
            return instance.Mouse.wheel_delta;
        }

        static sf::Vector2f GetMouseWorldPosition()
        {
            auto &instance = GetInstance();
            return instance.Mouse.world_position;
        }

        static sf::Vector2f GetMouseCanvasPosition()
        {
            auto &instance = GetInstance();
            return instance.Mouse.canvas_position;
        }

        static Input &GetInstance()
        {
            static Input instance;
            return instance;
        }
    };
}
