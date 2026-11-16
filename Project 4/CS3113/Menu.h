#ifndef MENU_H
#define MENU_H

#include "LevelA.h"

class Menu : public Scene
{
private:
    Entity *screen = nullptr;

public:
    Menu();
    Menu(Vector2 origin, const char *bgHexCode);
    ~Menu();

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif 