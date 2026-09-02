#include <Application.hpp>

class MainApplication : public PicSFML::Application
{
private:
public:
    MainApplication() : Application(sf::Vector2u(600, 300), "Application") 
    {
        GetWindow().setFramerateLimit(60);
    }

    void Start() override;
    void Update() override;
    void Events(const sf::Event &event) override;
    void DrawWorld() override;
    void DrawCanvas() override;
    void End() override;
};  

void MainApplication::Start()
{

}

void MainApplication::Update()
{
    
}

void MainApplication::Events(const sf::Event &event)
{

}

void MainApplication::DrawWorld()
{

}

void MainApplication::DrawCanvas()
{

}

void MainApplication::End()
{

}