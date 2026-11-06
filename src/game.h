#ifndef ROAD_OF_LIFE_GAME_H
#define ROAD_OF_LIFE_GAME_H

#include <string>

struct Role {
    std::string name;
    std::string sex;
    int age;
    int wisdom;
    int wealth;
    int experience;
};

// The same game can report changes to a graphical or console presentation.
class GameObserver {
public:
    virtual ~GameObserver() {}
    virtual void statusChanged(const Role& role) = 0;
    virtual void clearConversation() = 0;
};

int runGame(GameObserver* observer = nullptr);

#endif
