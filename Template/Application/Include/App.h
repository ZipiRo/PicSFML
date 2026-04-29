#include <Application.h>
#include <iostream>

class App : public Application
{
private:
public:
    App() : Application(sf::Vector2u(200, 200), "Template") 
    {
        window.setFramerateLimit(60);
    }

    void Update(float) override;
};

void App::Update(float delta_time)
{
    std::cout << 1.0f / delta_time << '\n';
}