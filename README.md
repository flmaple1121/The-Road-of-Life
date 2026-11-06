# The Road of Life

The Road of Life is a small, menu-driven C++ life simulation originally created as a course project. Choose one of three characters, pursue an education, manage a company, and watch your character's attributes change along the way.

**Original project completed: December 2, 2022.**

## Features

- Choose Chenxi, Haochen, or Yuyao, each with different starting attributes and possible outcomes.
- Select a university for a bachelor's degree and, later, a combined master's and doctoral program. During postgraduate studies, grade homework individually or automatically.
- Run a company: hire and dismiss staff, inspect and sort records, and adjust salaries. Employees age and eventually retire as time passes.
- Track age, wisdom, wealth, and experience; some outcomes are random.

The project uses a sequential list for staff records and linked stacks and a queue for homework grading. The English version retains the original game and data structures, with fixes for input handling, staff retirement, grading cleanup, and the staff capacity limit. It also supports clearing the screen on non-Windows terminals.

## Build and run

A C++11 compiler is required; there are no third-party dependencies. From the repository root, build with GCC:

```sh
g++ -std=c++11 -Wall -Wextra -pedantic "src/The Road of Life.cpp" -o road-of-life
```

Run `./road-of-life` on macOS or Linux. On Windows, the command produces `road-of-life.exe`; run `.\road-of-life.exe` in PowerShell. You can also open the source and headers in a C++ IDE and build the main `.cpp` file.

Choose options by entering their numbers. When hiring, enter each employee's name, sex, age, and monthly salary, separated by spaces; names must be a single word. Employee actions match the first record with the entered name. A game runs entirely in memory and does not save progress.

## Tests

With Python 3 and `g++` available, run the gameplay checks with:

```sh
python -m unittest discover -s tests -v
```

The tests compile a temporary executable and exercise education, grading, company management, input validation, and the staff limit.

## Project layout

```text
src/       English version and its data-structure headers
original/  Unmodified original Chinese source, headers, and Dev-C++ project
tests/     Gameplay checks
```

The files in `original/` are preserved byte-for-byte in their original GBK-compatible encoding and Windows line endings. If the Chinese text does not display correctly in an editor, open the files using GBK or GB18030. The original files are kept for reference; the build instructions above use the English version. Generated binaries, object files, and IDE session files are not tracked.
