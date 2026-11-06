// Exercise the same stream bridge used by the Windows chat window.
#include "../src/chat_win32.cpp"

#include <cassert>
#include <chrono>

struct Observer : GameObserver {
    Bridge& bridge;
    explicit Observer(Bridge& value) : bridge(value) {}
    void statusChanged(const Role& role) override
    {
        Event event(Event::Status);
        event.role = role;
        bridge.post(event);
    }
    void clearConversation() override { bridge.post(Event(Event::Clear)); }
};

void play(GameInput& input, GameOutput& output, Observer& observer)
{
    std::streambuf* oldIn = std::cin.rdbuf(&input);
    std::streambuf* oldOut = std::cout.rdbuf(&output);
    std::streambuf* oldErr = std::cerr.rdbuf(&output);
    std::cin.clear();
    runGame(&observer);
    output.publish();
    std::cin.rdbuf(oldIn);
    std::cout.rdbuf(oldOut);
    std::cerr.rdbuf(oldErr);
}

int main()
{
    Bridge bridge;
    GameOutput output(bridge);
    GameInput input(bridge, output);
    Observer observer(bridge);
    const char* lines[] = {"1", "1", "1", "2", "1", "6", "5"};
    std::thread game([&] { play(input, output, observer); });
    std::deque<Event> events;
    for (const char* line : lines) {
        bool waiting = false;
        for (int attempt = 0; attempt < 2000 && !waiting; ++attempt) {
            std::deque<Event> next = bridge.take();
            for (const Event& event : next)
                if (event.kind == Event::Ready) waiting = true;
            events.insert(events.end(), next.begin(), next.end());
            if (!waiting) std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        assert(waiting);
        input.submit(line);
    }
    game.join();
    std::deque<Event> tail = bridge.take();
    events.insert(events.end(), tail.begin(), tail.end());
    int ready = 0;
    std::vector<int> ages;
    std::string conversation;
    for (const Event& event : events) {
        if (event.kind == Event::Ready) ++ready;
        if (event.kind == Event::Text) conversation += event.text;
        if (event.kind == Event::Status) ages.push_back(event.role.age);
    }
    assert(ready == 7);
    assert((ages == std::vector<int>{18, 22, 23}));
    assert(conversation.find("Welcome to The Road of Life!") != std::string::npos);
    assert(conversation.find("Game over.") != std::string::npos);

    input.reset();
    output.reset();
    std::thread waiting([&] { play(input, output, observer); });
    input.close();
    waiting.join();
    events = bridge.take();
    bool closed = false;
    for (const Event& event : events)
        if (event.kind == Event::Text && event.text.find("Input closed.") != std::string::npos) closed = true;
    assert(closed);
}
