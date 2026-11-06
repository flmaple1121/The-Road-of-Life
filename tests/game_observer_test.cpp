#include "../src/game.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <vector>

struct Observer : GameObserver {
    std::vector<Role> statuses;
    int clears = 0;
    void statusChanged(const Role& role) override { statuses.push_back(role); }
    void clearConversation() override { ++clears; }
};

std::string play(const std::string& choices, Observer& observer)
{
    std::istringstream input(choices);
    std::ostringstream output;
    std::streambuf* oldIn = std::cin.rdbuf(input.rdbuf());
    std::streambuf* oldOut = std::cout.rdbuf(output.rdbuf());
    std::streambuf* oldErr = std::cerr.rdbuf(output.rdbuf());
    std::cin.clear();
    int result = runGame(&observer);
    std::cin.rdbuf(oldIn);
    std::cout.rdbuf(oldOut);
    std::cerr.rdbuf(oldErr);
    assert(result == 0);
    return output.str();
}

int main()
{
    Observer first;
    std::string output = play("1\n2\n1\n1\n2\n1\n1\n1\nTemp Male 24 2000\n6\n4\n", first);
    assert(output.find("complete your bachelor's degree") != std::string::npos);
    assert(output.find("1 year of work:") != std::string::npos);
    assert(first.statuses.size() == 3);
    assert(first.statuses[0].age == 18 && first.statuses[1].age == 22 && first.statuses[2].age == 23);
    assert(first.statuses[0].name == "Chenxi");

    Observer second;
    output = play("2\n1\n1\n2\n1\n3\n6\n4\n", second);
    assert(second.statuses.size() == 3);
    assert(second.statuses[0].name == "Haochen" && second.statuses[2].age == 23);
    assert(output.find("become the CEO") != std::string::npos);
    assert(output.find("Temp |") == std::string::npos);

    Observer cleared;
    play("1\n3\n4\n", cleared);
    assert(cleared.clears == 1);
}
