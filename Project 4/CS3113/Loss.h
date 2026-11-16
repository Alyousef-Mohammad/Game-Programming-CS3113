#ifndef LOSS_H
#define LOSS_H

#include "LevelA.h"

class Loss : public Scene
{
private:
    Entity *screen = nullptr;

public:
    Loss();
    Loss(Vector2 origin, const char *bgHexCode);
    ~Loss();

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif 