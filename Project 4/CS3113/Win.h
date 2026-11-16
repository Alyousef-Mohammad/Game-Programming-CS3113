#ifndef WIN_H
#define WIN_H

#include "LevelA.h"

class Win : public Scene
{
private:
    Entity *screen = nullptr;

public:
    Win();
    Win(Vector2 origin, const char *bgHexCode);
    ~Win();

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif 