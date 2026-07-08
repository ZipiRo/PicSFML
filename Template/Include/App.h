#include <Application.h>
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>

using namespace sf;

class App : public Application
{
private:
public:
    App() : Application(sf::Vector2u(1600, 900), "PicSFML_app") 
    {
        GetWindow().setFramerateLimit(60);
    }


    void Start() override;
    void Update(float) override;
    void DrawWorld() override;
};  

void App::Start()
{

}

void App::Update(float delta_time)
{

}

void App::DrawWorld()
{

}