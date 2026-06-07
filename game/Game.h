#include <engine/EntryPoint.h>
#include <engine/core/Logger.h>

class GameApplication : public Walrus::Application
{
public:
    GameApplication(ApplicationCommandLineArgs args)
        : Application(
              ApplicationSpecification{
                  .Name = "Walrus Engine Test", .StartX = 100, .StartY = 100, .Width = 1280, .Height = 720, .Args = args })
    {
    }

protected:
    bool OnInitialize() override;
    bool OnUpdate(float deltaTime) override;
    bool OnRender(float deltaTime) override;
    void OnResize(u32 width, u32 height) override;
};