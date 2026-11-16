#ifndef LEVELB_H
#define LEVELB_H

#include "LevelA.h"

class LevelB : public Scene
{
private:
    // Level 2 -- tileset.png is 13 tiles in one row (see level_maps.h)
    unsigned int mLevelData[LEVEL_WIDTH * LEVEL_HEIGHT] = {
        4, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 4,
        5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
        5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
        5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
        5, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
        5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
        5, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
        5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
        5, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 10, 11, 0, 0, 6,
        5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 13, 0, 0, 6,
        5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
        5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
        5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
        5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
        5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
        5, 10, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
        5, 12, 13, 0, 0, 2, 1, 3, 0, 0, 0, 0, 0, 0, 0, 6,
        5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
        5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 6,
        5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 6,
        5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 6,
        5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 6,
        5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 6,
        5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
        5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
        4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4
    };


    Entity *mUpdraft = nullptr;
    Entity *ghost = nullptr;
    Entity *spikes[9] = {};
    bool pipeentered = false;

    Sound updraft;

public:
    LevelB();
    LevelB(Vector2 origin, const char *bgHexCode);
    ~LevelB();

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif // LEVELB_H