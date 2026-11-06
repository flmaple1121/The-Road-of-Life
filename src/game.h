#ifndef ROAD_OF_LIFE_GAME_H
#define ROAD_OF_LIFE_GAME_H

#include <string>
#include <vector>

const int MIN_EMPLOYEE_AGE = 18;
const int MAX_EMPLOYEE_AGE = 59;
const int MAX_MONTHLY_SALARY = 1000000;

struct Staff {
    std::string name;
    std::string sex;
    int age;
    int worktime;
    int salary;
};

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

// The Windows presentation collects a complete operation before the game commits it.
class StaffForms {
public:
    virtual ~StaffForms() {}
    virtual bool hire(int limit, std::vector<Staff>& employees) = 0;
    virtual bool dismiss(const std::vector<Staff>& employees, int& index) = 0;
    virtual bool salaryChange(const std::vector<Staff>& employees, int& index, int& change) = 0;
};

bool parseIntegerInRange(const std::string& text, int minimum, int maximum, int& value);
bool validateNewEmployee(const Staff& employee, std::string& error);
int runGame(GameObserver* observer = nullptr, StaffForms* forms = nullptr);

#endif
