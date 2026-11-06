# The Road of Life

The Road of Life is a C++ life simulation that began as a course project. Choose a character, go to university, run a company, and see how your decisions shape your journey.

**Original project completed: December 2, 2022.**

**Chat interface extension started: November 6, 2026.**

The Windows chat edition presents the game as a conversation with a life guide, with a retro instant-messaging look and a live character panel. The English dialogue has also been revised for a more natural reading experience.

## Play the chat edition on Windows

[Download the Windows x64 chat edition](https://github.com/flmaple1121/The-Road-of-Life/releases/latest/download/The-Road-of-Life-Chat.exe) and open the executable; no installation is required.

To build it yourself, use a MinGW-w64 C++ compiler that supports C++11 and POSIX threads. From the repository folder, run:

```powershell
.\build-windows.bat
& ".\build\The Road of Life - Chat.exe"
```

Alternatively, open `build\The Road of Life - Chat.exe` from File Explorer after building. The program uses the Windows API and has no third-party runtime dependencies.

Enter a menu number or a requested answer in the message box and press **Enter** or **Send**. Scroll the conversation to revisit earlier messages. Your character's age, wisdom, wealth, and experience appear at the top and update as the story progresses. After ending a journey, select **New game** to start again.

Hiring, dismissing an employee, and changing a salary open separate forms. Choose sex from the list and enter a name (1-40 characters), an age from 18 to 59, and a monthly salary from 0 to 1,000,000. Salary changes can be positive or negative as long as the resulting salary remains within that range. Commas such as `20,000` are accepted in numeric fields. Employee lists show each record separately, including people with the same name.

Each form can be canceled before the change is applied. When canceling a batch hire after completing some records, choose whether to hire the completed employees, discard them, or continue editing. A fully filled current form is included, even if Next has not been pressed. Sorting can also be canceled with option `0`. Progress stays in memory for the current game and is not saved.

## Play in a terminal

The chat interface and the terminal version share the same C++ game logic. To build the terminal version on Windows, macOS, or Linux with GCC:

```sh
g++ -std=c++11 -Wall -Wextra -pedantic "src/The Road of Life.cpp" -o road-of-life
```

Run `./road-of-life` on macOS or Linux, or `.\road-of-life.exe` in Windows PowerShell. The terminal version does not require the Windows chat interface. In the terminal, enter employee fields individually or together with spaces or comma-and-space separators; names are single words. Type `cancel` at a field prompt to return without applying that operation, and confirm changes when prompted.

## Features

- Play as Chenxi, Haochen, or Yuyao, each with different starting attributes and possible outcomes.
- Choose a university, complete a bachelor's degree, and pursue a combined master's and doctoral program. As a teaching assistant, grade homework yourself or automatically.
- Hire and dismiss employees, sort staff records, and adjust salaries. Employees age and eventually retire as time passes.
- Watch your character's attributes change. Some outcomes are random.

The game uses a sequential list for staff records and linked stacks and a queue for homework grading.

## Tests

With Python 3 and `g++` available, run:

```sh
python -m unittest discover -s tests -v
```

The tests exercise gameplay, field validation, cancellation and partial batch hiring, status updates, restarting, and the staff limit. On Windows, they also check the staff forms, chat message bridge, and shutdown while waiting for input.

## Project layout

```text
src/                 Shared game logic, data structures, and Windows chat interface
original/            Unmodified original Chinese source, headers, and Dev-C++ project
tests/               Gameplay and chat bridge checks
build-windows.bat    Build the Windows chat edition
```

The files in `original/` are preserved byte-for-byte in their original GBK-compatible encoding and Windows line endings. If the Chinese text does not display correctly in an editor, open the files using GBK or GB18030. The original files are kept for reference; the build instructions above use the updated English version. Generated binaries, object files, and IDE session files are not tracked.
