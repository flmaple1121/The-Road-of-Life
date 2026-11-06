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

int forwardedCharacters = 0;
LRESULT CALLBACK originalInput(HWND, UINT, WPARAM, LPARAM)
{
    ++forwardedCharacters;
    return 42;
}

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
    const char* lines[] = {"1", "1", "1", "2", "1", "6", "4"};
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
    assert(conversation.find("View your attributes") == std::string::npos);
    assert(conversation.find("3. Clear the conversation") != std::string::npos);
    assert(conversation.find("4. End your journey") != std::string::npos);

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

    // Enter's character message must not reach the single-line edit control.
    oldInputProc = originalInput;
    assert(inputProcedure(nullptr, WM_CHAR, L'\r', 0) == 0);
    assert(inputProcedure(nullptr, WM_CHAR, L'\n', 0) == 0);
    assert(forwardedCharacters == 0);
    assert(inputProcedure(nullptr, WM_CHAR, L'7', 0) == 42);
    assert(forwardedCharacters == 1);

    // A form request travels to the window thread and returns completed data.
    ChatApp formApp;
    std::vector<Staff> hires;
    bool accepted=false;
    std::thread requester([&] { accepted=formApp.hire(5,hires); });
    std::shared_ptr<FormRequest> request;
    for(int attempt=0;attempt<2000 && !request;++attempt)
    {
        std::deque<Event> next=formApp.bridge.take();
        for(const Event& event : next) if(event.kind==Event::Form) request=event.form;
        if(!request) std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    assert(request && request->action==FormRequest::Hire && request->limit==5);
    {
        std::lock_guard<std::mutex> lock(request->mutex);
        request->hires.push_back(Staff{"Alice","Female",25,0,5000});
        request->accepted=true;
        request->completed=true;
    }
    request->ready.notify_one();
    requester.join();
    assert(accepted && hires.size()==1 && hires[0].name=="Alice");

    // Closing the app while a form is pending must unblock the game thread.
    ChatApp closingApp;
    bool closingAccepted=true;
    std::thread pending([&] { std::vector<Staff> values; closingAccepted=closingApp.hire(5,values); });
    request.reset();
    for(int attempt=0;attempt<2000 && !request;++attempt)
    {
        std::deque<Event> next=closingApp.bridge.take();
        for(const Event& event : next) if(event.kind==Event::Form) request=event.form;
        if(!request) std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    assert(request);
    closingApp.stopGame();
    pending.join();
    assert(!closingAccepted);
}
