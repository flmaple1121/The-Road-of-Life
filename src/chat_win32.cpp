// Native chat presentation added on November 6, 2026.
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif
#define _WIN32_WINNT 0x0600
#include <windows.h>
#include <windowsx.h>

#include <algorithm>
#include <condition_variable>
#include <deque>
#include <iostream>
#include <mutex>
#include <streambuf>
#include <string>
#include <thread>
#include <vector>

#include "game.h"

namespace {

const UINT WM_GAME_EVENT = WM_APP + 1;
const int ID_INPUT = 101;
const int ID_SEND = 102;
const int ID_NEW_GAME = 103;
int dpi = 96;
WNDPROC oldInputProc = nullptr;

LRESULT CALLBACK inputProcedure(HWND target, UINT message, WPARAM wparam, LPARAM lparam)
{
    if (message == WM_KEYDOWN && wparam == VK_RETURN) {
        SendMessageW(GetParent(target), WM_COMMAND, MAKEWPARAM(ID_SEND, BN_CLICKED), 0);
        return 0;
    }
    if (message == WM_MOUSEWHEEL)
        return SendMessageW(GetParent(target), message, wparam, lparam);
    return CallWindowProcW(oldInputProc, target, message, wparam, lparam);
}

int px(int value) { return MulDiv(value, dpi, 96); }

std::wstring widen(const std::string& text)
{
    if (text.empty()) return std::wstring();
    int count = MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0);
    std::wstring result(count, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), &result[0], count);
    return result;
}

std::string narrow(const std::wstring& text)
{
    if (text.empty()) return std::string();
    int count = WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0, nullptr, nullptr);
    std::string result(count, '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), &result[0], count, nullptr, nullptr);
    return result;
}

std::wstring trim(const std::wstring& value)
{
    const std::wstring whitespace = L" \t\r\n";
    const std::wstring::size_type first = value.find_first_not_of(whitespace);
    if (first == std::wstring::npos) return std::wstring();
    return value.substr(first, value.find_last_not_of(whitespace) - first + 1);
}

struct Event {
    enum Kind { Text, Status, Clear, Ready, Finished } kind;
    std::string text;
    Role role;
    explicit Event(Kind value) : kind(value), role() {}
};

// The game runs on one worker; only the window thread touches Windows controls.
struct Bridge {
    HWND window = nullptr;
    std::mutex mutex;
    std::deque<Event> events;

    void post(const Event& event)
    {
        {
            std::lock_guard<std::mutex> lock(mutex);
            events.push_back(event);
        }
        PostMessageW(window, WM_GAME_EVENT, 0, 0);
    }

    std::deque<Event> take()
    {
        std::lock_guard<std::mutex> lock(mutex);
        std::deque<Event> result;
        result.swap(events);
        return result;
    }
};

class GameOutput : public std::streambuf {
public:
    explicit GameOutput(Bridge& bridge) : bridge_(bridge) {}

    void publish()
    {
        if (pending_.empty()) return;
        Event event(Event::Text);
        event.text.swap(pending_);
        bridge_.post(event);
    }

    void reset() { pending_.clear(); }

protected:
    int_type overflow(int_type character) override
    {
        if (!traits_type::eq_int_type(character, traits_type::eof()))
            pending_.push_back(traits_type::to_char_type(character));
        return traits_type::not_eof(character);
    }

    std::streamsize xsputn(const char* data, std::streamsize count) override
    {
        pending_.append(data, static_cast<std::size_t>(count));
        return count;
    }

    // endl should not split a menu into separate chat messages.
    int sync() override { return 0; }

private:
    Bridge& bridge_;
    std::string pending_;
};

class GameInput : public std::streambuf {
public:
    GameInput(Bridge& bridge, GameOutput& output)
        : bridge_(bridge), output_(output), closed_(false) { setg(nullptr, nullptr, nullptr); }

    void submit(const std::string& line)
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            lines_.push_back(line);
        }
        available_.notify_one();
    }

    void close()
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            closed_ = true;
        }
        available_.notify_one();
    }

    void reset()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        lines_.clear();
        current_.clear();
        closed_ = false;
        setg(nullptr, nullptr, nullptr);
    }

protected:
    int_type underflow() override
    {
        if (gptr() && gptr() < egptr()) return traits_type::to_int_type(*gptr());
        output_.publish();
        bridge_.post(Event(Event::Ready));
        std::unique_lock<std::mutex> lock(mutex_);
        available_.wait(lock, [this] { return closed_ || !lines_.empty(); });
        if (closed_) return traits_type::eof();
        current_ = lines_.front() + '\n';
        lines_.pop_front();
        char* start = &current_[0];
        setg(start, start, start + current_.size());
        return traits_type::to_int_type(*gptr());
    }

private:
    Bridge& bridge_;
    GameOutput& output_;
    std::mutex mutex_;
    std::condition_variable available_;
    std::deque<std::string> lines_;
    std::string current_;
    bool closed_;
};

struct Message {
    bool fromPlayer;
    std::wstring text;
};

class ChatApp : public GameObserver {
public:
    ChatApp() : output(bridge), input(bridge, output), window(nullptr), chat(nullptr),
        edit(nullptr), send(nullptr), newGame(nullptr), role(), hasRole(false), finished(false),
        scroll(0), contentHeight(0), bodyFont(nullptr), smallFont(nullptr),
        boldFont(nullptr), titleFont(nullptr), valueFont(nullptr) {}

    void initialize(HWND hwnd);
    void startGame();
    void stopGame();
    void submit();
    void processEvents();
    void layout();
    void paint(HDC dc);
    void paintChat(HDC dc);
    void updateScroll(bool toBottom);
    void scrollChat(int position);
    int measureMessages(HDC dc, int width, bool draw);
    void drawButton(const DRAWITEMSTRUCT& item);
    void addMessage(bool fromPlayer, const std::wstring& text);

    void statusChanged(const Role& value) override
    {
        Event event(Event::Status);
        event.role = value;
        bridge.post(event);
    }

    void clearConversation() override { bridge.post(Event(Event::Clear)); }

    Bridge bridge;
    GameOutput output;
    GameInput input;
    std::thread worker;
    HWND window, chat, edit, send, newGame;
    Role role;
    bool hasRole, finished;
    int scroll, contentHeight;
    std::vector<Message> messages;
    HFONT bodyFont, smallFont, boldFont, titleFont, valueFont;
};

ChatApp* app = nullptr;

HFONT makeFont(int size, int weight = FW_NORMAL)
{
    return CreateFontW(-px(size), 0, 0, 0, weight, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
}

void fill(HDC dc, const RECT& rect, COLORREF color)
{
    HBRUSH brush = CreateSolidBrush(color);
    FillRect(dc, &rect, brush);
    DeleteObject(brush);
}

void rounded(HDC dc, const RECT& rect, int radius, COLORREF color, COLORREF border)
{
    HBRUSH brush = CreateSolidBrush(color);
    HPEN pen = CreatePen(PS_SOLID, 1, border);
    HGDIOBJ oldBrush = SelectObject(dc, brush);
    HGDIOBJ oldPen = SelectObject(dc, pen);
    RoundRect(dc, rect.left, rect.top, rect.right, rect.bottom, radius, radius);
    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    DeleteObject(brush);
    DeleteObject(pen);
}

void label(HDC dc, HFONT font, COLORREF color, RECT rect, const std::wstring& text, UINT flags)
{
    HGDIOBJ old = SelectObject(dc, font);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, color);
    DrawTextW(dc, text.c_str(), -1, &rect, flags | DT_NOPREFIX);
    SelectObject(dc, old);
}

void ChatApp::initialize(HWND hwnd)
{
    window = hwnd;
    bridge.window = hwnd;
    bodyFont = makeFont(15);
    smallFont = makeFont(12);
    boldFont = makeFont(14, FW_SEMIBOLD);
    titleFont = makeFont(21, FW_SEMIBOLD);
    valueFont = makeFont(20, FW_SEMIBOLD);

    chat = CreateWindowExW(0, L"RoadLifeChat", L"", WS_CHILD | WS_VISIBLE | WS_VSCROLL,
        0, 0, 0, 0, hwnd, nullptr, GetModuleHandleW(nullptr), nullptr);
    edit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
        0, 0, 0, 0, hwnd, reinterpret_cast<HMENU>(ID_INPUT), GetModuleHandleW(nullptr), nullptr);
    send = CreateWindowExW(0, L"BUTTON", L"Send", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW,
        0, 0, 0, 0, hwnd, reinterpret_cast<HMENU>(ID_SEND), GetModuleHandleW(nullptr), nullptr);
    newGame = CreateWindowExW(0, L"BUTTON", L"New game", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW,
        0, 0, 0, 0, hwnd, reinterpret_cast<HMENU>(ID_NEW_GAME), GetModuleHandleW(nullptr), nullptr);
    SendMessageW(edit, WM_SETFONT, reinterpret_cast<WPARAM>(bodyFont), TRUE);
    SendMessageW(edit, EM_SETLIMITTEXT, 1024, 0);
    oldInputProc = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(edit, GWLP_WNDPROC,
        reinterpret_cast<LONG_PTR>(inputProcedure)));
    layout();
    startGame();
}

void ChatApp::startGame()
{
    if (worker.joinable()) worker.join();
    input.reset();
    output.reset();
    messages.clear();
    scroll = 0;
    hasRole = false;
    finished = false;
    EnableWindow(edit, FALSE);
    EnableWindow(send, FALSE);
    EnableWindow(newGame, FALSE);
    SetWindowTextW(edit, L"");
    InvalidateRect(window, nullptr, TRUE);
    updateScroll(true);
    worker = std::thread([this] {
        std::streambuf* oldIn = std::cin.rdbuf(&input);
        std::streambuf* oldOut = std::cout.rdbuf(&output);
        std::streambuf* oldErr = std::cerr.rdbuf(&output);
        std::cin.clear();
        runGame(this);
        output.publish();
        std::cin.rdbuf(oldIn);
        std::cout.rdbuf(oldOut);
        std::cerr.rdbuf(oldErr);
        bridge.post(Event(Event::Finished));
    });
}

void ChatApp::stopGame()
{
    input.close();
    if (worker.joinable()) worker.join();
}

void ChatApp::submit()
{
    if (!IsWindowEnabled(edit)) return;
    int length = GetWindowTextLengthW(edit);
    std::vector<wchar_t> buffer(static_cast<std::size_t>(length) + 1);
    GetWindowTextW(edit, buffer.data(), length + 1);
    std::wstring text = trim(std::wstring(buffer.data()));
    if (text.empty()) return;
    addMessage(true, text);
    SetWindowTextW(edit, L"");
    EnableWindow(edit, FALSE);
    EnableWindow(send, FALSE);
    input.submit(narrow(text));
}

void ChatApp::processEvents()
{
    std::deque<Event> events = bridge.take();
    for (std::deque<Event>::const_iterator it = events.begin(); it != events.end(); ++it) {
        if (it->kind == Event::Text) addMessage(false, widen(it->text));
        else if (it->kind == Event::Status) {
            role = it->role;
            hasRole = true;
            InvalidateRect(window, nullptr, FALSE);
        } else if (it->kind == Event::Clear) {
            messages.clear();
            updateScroll(true);
        } else if (it->kind == Event::Ready) {
            EnableWindow(edit, TRUE);
            EnableWindow(send, TRUE);
            SetFocus(edit);
        } else if (it->kind == Event::Finished) {
            finished = true;
            EnableWindow(edit, FALSE);
            EnableWindow(send, FALSE);
            EnableWindow(newGame, TRUE);
            InvalidateRect(window, nullptr, FALSE);
        }
    }
}

void ChatApp::addMessage(bool fromPlayer, const std::wstring& value)
{
    std::wstring text = trim(value);
    if (text.empty()) return;
    Message message = {fromPlayer, text};
    messages.push_back(message);
    updateScroll(true);
}

void ChatApp::layout()
{
    if (!chat) return;
    RECT rect;
    GetClientRect(window, &rect);
    int width = rect.right;
    int height = rect.bottom;
    MoveWindow(newGame, width - px(132), px(20), px(104), px(34), TRUE);
    MoveWindow(chat, px(16), px(211), std::max(1, width - px(32)),
        std::max(1, height - px(302)), TRUE);
    MoveWindow(edit, px(25), height - px(72), std::max(1, width - px(156)), px(36), TRUE);
    MoveWindow(send, width - px(119), height - px(72), px(94), px(36), TRUE);
    updateScroll(false);
}

int ChatApp::measureMessages(HDC dc, int width, bool draw)
{
    int y = px(17) - (draw ? scroll : 0);
    int maxBubble = std::max(px(120), std::min(px(540), width - px(98)));
    const int padX = px(14), padY = px(10), avatar = px(34);
    HGDIOBJ oldFont = SelectObject(dc, bodyFont);
    for (std::size_t i = 0; i < messages.size(); ++i) {
        const Message& message = messages[i];
        RECT measured = {0, 0, maxBubble - 2 * padX, 0};
        DrawTextW(dc, message.text.c_str(), -1, &measured,
            DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_NOPREFIX);
        int bubbleWidth = std::min(maxBubble, std::max(px(125), static_cast<int>(measured.right) + 2 * padX));
        int bubbleHeight = std::max(px(40), static_cast<int>(measured.bottom) + 2 * padY);
        int bubbleX = message.fromPlayer ? width - px(54) - bubbleWidth : px(54);
        RECT bubble = {bubbleX, y + px(21), bubbleX + bubbleWidth, y + px(21) + bubbleHeight};
        if (draw) {
            RECT avatarRect = message.fromPlayer
                ? RECT{width - px(42), y + px(21), width - px(42) + avatar, y + px(21) + avatar}
                : RECT{px(10), y + px(21), px(10) + avatar, y + px(21) + avatar};
            rounded(dc, avatarRect, px(11), message.fromPlayer ? RGB(105, 183, 224) : RGB(94, 162, 207),
                message.fromPlayer ? RGB(105, 183, 224) : RGB(94, 162, 207));
            label(dc, boldFont, RGB(255, 255, 255), avatarRect, message.fromPlayer ? L"Y" : L"L",
                DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            RECT name = {bubbleX, y, bubbleX + bubbleWidth, y + px(19)};
            label(dc, smallFont, RGB(105, 135, 152), name, message.fromPlayer ? L"You" : L"Life Guide",
                (message.fromPlayer ? DT_RIGHT : DT_LEFT) | DT_VCENTER | DT_SINGLELINE);
            rounded(dc, bubble, px(13), message.fromPlayer ? RGB(216, 241, 253) : RGB(255, 255, 255),
                message.fromPlayer ? RGB(185, 224, 242) : RGB(224, 236, 243));
            RECT textRect = {bubble.left + padX, bubble.top + padY,
                bubble.right - padX, bubble.bottom - padY};
            SetTextColor(dc, RGB(42, 65, 81));
            SetBkMode(dc, TRANSPARENT);
            DrawTextW(dc, message.text.c_str(), -1, &textRect, DT_WORDBREAK | DT_EDITCONTROL | DT_NOPREFIX);
        }
        y += px(21) + bubbleHeight + px(18);
    }
    SelectObject(dc, oldFont);
    return y + (draw ? scroll : 0) + px(5);
}

void ChatApp::updateScroll(bool toBottom)
{
    if (!chat) return;
    RECT rect;
    GetClientRect(chat, &rect);
    HDC dc = GetDC(chat);
    contentHeight = measureMessages(dc, rect.right, false);
    ReleaseDC(chat, dc);
    int limit = std::max(0, contentHeight - static_cast<int>(rect.bottom));
    scroll = toBottom ? limit : std::min(scroll, limit);
    SCROLLINFO info = {sizeof(info), SIF_PAGE | SIF_RANGE | SIF_POS, 0,
        std::max(0, contentHeight - 1), static_cast<UINT>(std::max(0L, rect.bottom)), scroll, 0};
    SetScrollInfo(chat, SB_VERT, &info, TRUE);
    InvalidateRect(chat, nullptr, TRUE);
}

void ChatApp::scrollChat(int position)
{
    RECT rect;
    GetClientRect(chat, &rect);
    scroll = std::max(0, std::min(position, std::max(0, contentHeight - static_cast<int>(rect.bottom))));
    SetScrollPos(chat, SB_VERT, scroll, TRUE);
    InvalidateRect(chat, nullptr, TRUE);
}

void ChatApp::paintChat(HDC dc)
{
    RECT rect;
    GetClientRect(chat, &rect);
    fill(dc, rect, RGB(245, 250, 253));
    measureMessages(dc, rect.right, true);
}

void ChatApp::paint(HDC dc)
{
    RECT client;
    GetClientRect(window, &client);
    const int width = client.right, height = client.bottom;
    fill(dc, client, RGB(239, 247, 252));
    for (int y = 0; y < px(88); ++y) {
        int r = 118 + MulDiv(66, y, px(88));
        int g = 190 + MulDiv(40, y, px(88));
        int b = 234 + MulDiv(16, y, px(88));
        RECT line = {0, y, width, y + 1};
        fill(dc, line, RGB(r, g, b));
    }
    RECT logo = {px(22), px(19), px(68), px(65)};
    rounded(dc, logo, px(14), RGB(78, 156, 211), RGB(245, 252, 255));
    label(dc, titleFont, RGB(255, 255, 255), logo, L"R", DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    RECT title = {px(82), px(13), width - px(150), px(43)};
    label(dc, titleFont, RGB(24, 76, 113), title, L"The Road of Life", DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    RECT subtitle = {px(83), px(45), width - px(150), px(67)};
    label(dc, smallFont, RGB(49, 112, 151), subtitle, L"Life Guide  \u2022  online", DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    RECT edition = {width - px(218), px(59), width - px(26), px(78)};
    label(dc, smallFont, RGB(53, 116, 151), edition, L"CHAT EDITION  \u00b7  2026.11.06", DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

    RECT card = {px(16), px(96), width - px(16), px(169)};
    rounded(dc, card, px(12), RGB(255, 255, 255), RGB(207, 228, 239));
    RECT heading = {px(30), px(102), px(214), px(120)};
    label(dc, smallFont, RGB(114, 149, 169), heading, L"YOUR CHARACTER", DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    RECT name = {px(30), px(120), px(217), px(153)};
    label(dc, valueFont, RGB(38, 79, 108), name, hasRole ? widen(role.name) : L"Choose a character",
        DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    const wchar_t* names[] = {L"AGE", L"WISDOM", L"WEALTH", L"EXPERIENCE"};
    int values[] = {role.age, role.wisdom, role.wealth, role.experience};
    int left = px(230), available = std::max(1, width - px(260));
    for (int i = 0; i < 4; ++i) {
        int x = left + available * i / 4;
        RECT line = {x - px(12), px(110), x - px(11), px(153)};
        fill(dc, line, RGB(231, 240, 245));
        RECT statLabel = {x, px(103), x + available / 4 - px(4), px(122)};
        label(dc, smallFont, RGB(114, 149, 169), statLabel, names[i], DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        RECT statValue = {x, px(122), x + available / 4 - px(4), px(153)};
        label(dc, valueFont, RGB(38, 79, 108), statValue,
            hasRole ? std::to_wstring(values[i]) : L"--", DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    }
    RECT conversation = {px(22), px(181), width / 2, px(205)};
    label(dc, boldFont, RGB(48, 91, 118), conversation, L"Conversation", DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    RECT presence = {width / 2, px(181), width - px(24), px(205)};
    label(dc, smallFont, RGB(112, 155, 177), presence, finished ? L"JOURNEY ENDED" : L"LIFE GUIDE IS ONLINE",
        DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
    RECT composer = {0, height - px(88), width, height};
    fill(dc, composer, RGB(255, 255, 255));
    RECT divider = {0, height - px(89), width, height - px(88)};
    fill(dc, divider, RGB(211, 230, 239));
    RECT hint = {px(26), height - px(29), width - px(25), height - px(8)};
    label(dc, smallFont, RGB(132, 158, 173), hint, L"Type a number or reply, then press Enter.",
        DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}

void ChatApp::drawButton(const DRAWITEMSTRUCT& item)
{
    bool disabled = (item.itemState & ODS_DISABLED) != 0;
    bool pressed = (item.itemState & ODS_SELECTED) != 0;
    RECT r = item.rcItem;
    COLORREF background = disabled ? RGB(224, 236, 242) : (pressed ? RGB(77, 153, 205) : RGB(99, 178, 224));
    rounded(item.hDC, r, px(9), background, disabled ? RGB(210, 225, 234) : RGB(77, 154, 202));
    wchar_t text[32];
    GetWindowTextW(item.hwndItem, text, 32);
    label(item.hDC, boldFont, disabled ? RGB(154, 177, 190) : RGB(255, 255, 255), r, text,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    if (item.itemState & ODS_FOCUS) {
        InflateRect(&r, -px(4), -px(4));
        DrawFocusRect(item.hDC, &r);
    }
}

LRESULT CALLBACK chatProcedure(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    switch (message) {
    case WM_ERASEBKGND: return 1;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(hwnd, &ps);
        RECT rect;
        GetClientRect(hwnd, &rect);
        HDC memory = CreateCompatibleDC(dc);
        HBITMAP bitmap = CreateCompatibleBitmap(dc, std::max(1L, rect.right), std::max(1L, rect.bottom));
        HGDIOBJ old = SelectObject(memory, bitmap);
        app->paintChat(memory);
        BitBlt(dc, 0, 0, rect.right, rect.bottom, memory, 0, 0, SRCCOPY);
        SelectObject(memory, old);
        DeleteObject(bitmap);
        DeleteDC(memory);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_SIZE: app->updateScroll(false); return 0;
    case WM_LBUTTONDOWN: SetFocus(hwnd); return 0;
    case WM_MOUSEWHEEL:
        app->scrollChat(app->scroll - GET_WHEEL_DELTA_WPARAM(wparam) * px(45) / WHEEL_DELTA);
        return 0;
    case WM_KEYDOWN:
        if (wparam == VK_PRIOR) app->scrollChat(app->scroll - px(180));
        else if (wparam == VK_NEXT) app->scrollChat(app->scroll + px(180));
        else if (wparam == VK_UP) app->scrollChat(app->scroll - px(35));
        else if (wparam == VK_DOWN) app->scrollChat(app->scroll + px(35));
        return 0;
    case WM_VSCROLL: {
        SCROLLINFO info = {sizeof(info), SIF_ALL, 0, 0, 0, 0, 0};
        GetScrollInfo(hwnd, SB_VERT, &info);
        int target = app->scroll;
        switch (LOWORD(wparam)) {
        case SB_LINEUP: target -= px(35); break;
        case SB_LINEDOWN: target += px(35); break;
        case SB_PAGEUP: target -= static_cast<int>(info.nPage); break;
        case SB_PAGEDOWN: target += static_cast<int>(info.nPage); break;
        case SB_THUMBTRACK: target = info.nTrackPos; break;
        case SB_TOP: target = 0; break;
        case SB_BOTTOM: target = info.nMax; break;
        }
        app->scrollChat(target);
        return 0;
    }
    }
    return DefWindowProcW(hwnd, message, wparam, lparam);
}

LRESULT CALLBACK windowProcedure(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    switch (message) {
    case WM_CREATE: app->initialize(hwnd); return 0;
    case WM_SIZE: app->layout(); return 0;
    case WM_GETMINMAXINFO: {
        MINMAXINFO* info = reinterpret_cast<MINMAXINFO*>(lparam);
        info->ptMinTrackSize.x = px(710);
        info->ptMinTrackSize.y = px(610);
        return 0;
    }
    case WM_COMMAND:
        if (LOWORD(wparam) == ID_SEND) app->submit();
        else if (LOWORD(wparam) == ID_NEW_GAME && app->finished) app->startGame();
        return 0;
    case WM_GAME_EVENT: app->processEvents(); return 0;
    case WM_MOUSEWHEEL:
        app->scrollChat(app->scroll - GET_WHEEL_DELTA_WPARAM(wparam) * px(45) / WHEEL_DELTA);
        return 0;
    case WM_DRAWITEM:
        app->drawButton(*reinterpret_cast<DRAWITEMSTRUCT*>(lparam));
        return TRUE;
    case WM_ERASEBKGND: return 1;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(hwnd, &ps);
        RECT rect;
        GetClientRect(hwnd, &rect);
        HDC memory = CreateCompatibleDC(dc);
        HBITMAP bitmap = CreateCompatibleBitmap(dc, std::max(1L, rect.right), std::max(1L, rect.bottom));
        HGDIOBJ old = SelectObject(memory, bitmap);
        app->paint(memory);
        BitBlt(dc, 0, 0, rect.right, rect.bottom, memory, 0, 0, SRCCOPY);
        SelectObject(memory, old);
        DeleteObject(bitmap);
        DeleteDC(memory);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_CLOSE: app->stopGame(); DestroyWindow(hwnd); return 0;
    case WM_DESTROY: PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(hwnd, message, wparam, lparam);
}

} // namespace

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show)
{
    typedef BOOL (WINAPI *SetDpiAware)();
    SetDpiAware setDpiAware = reinterpret_cast<SetDpiAware>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetProcessDPIAware"));
    if (setDpiAware) setDpiAware();
    HDC screen = GetDC(nullptr);
    dpi = GetDeviceCaps(screen, LOGPIXELSX);
    ReleaseDC(nullptr, screen);
    WNDCLASSW chatClass = {};
    chatClass.lpfnWndProc = chatProcedure;
    chatClass.hInstance = instance;
    chatClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    chatClass.lpszClassName = L"RoadLifeChat";
    RegisterClassW(&chatClass);
    WNDCLASSW mainClass = {};
    mainClass.lpfnWndProc = windowProcedure;
    mainClass.hInstance = instance;
    mainClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    mainClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    mainClass.lpszClassName = L"RoadLifeWindow";
    RegisterClassW(&mainClass);
    ChatApp application;
    app = &application;
    RECT workArea = {};
    int initialWidth = px(930), initialHeight = px(790);
    if (SystemParametersInfoW(SPI_GETWORKAREA, 0, &workArea, 0)) {
        initialWidth = std::min(initialWidth, static_cast<int>(workArea.right - workArea.left) - px(20));
        initialHeight = std::min(initialHeight, static_cast<int>(workArea.bottom - workArea.top) - px(20));
    }
    HWND window = CreateWindowExW(0, mainClass.lpszClassName, L"The Road of Life",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, initialWidth, initialHeight,
        nullptr, nullptr, instance, nullptr);
    if (!window) return 1;
    ShowWindow(window, show);
    UpdateWindow(window);
    MSG message;
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    DeleteObject(application.bodyFont);
    DeleteObject(application.smallFont);
    DeleteObject(application.boldFont);
    DeleteObject(application.titleFont);
    DeleteObject(application.valueFont);
    return static_cast<int>(message.wParam);
}
