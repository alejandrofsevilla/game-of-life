# ![LinuxWorkflow](https://github.com/alejandrofsevilla/game-of-life/actions/workflows/Linux.yml/badge.svg) ![WindowsWorkflow](https://github.com/alejandrofsevilla/game-of-life/actions/workflows/Windows.yml/badge.svg)
# Game of Life

Desktop application using [SFML](https://www.sfml-dev.org/) to visualize [Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) and other [life-like cellular automata](https://en.wikipedia.org/wiki/Life-like_cellular_automaton).

![v1 0 2](https://github.com/alejandrofsevilla/game-of-life/assets/110661590/10e39bde-3700-42b9-80a7-d921e26b1065)

## About the Game of Life
Conway's Game is a cellular automata invented by the British mathematician John Horton Conway in 1970. It is a zero-player game that evolves based on its initial state. Cells in a grid live, die, or multiply based on the following rules:
- Any live cell with fewer than two living neighbors dies (underpopulation).
- Any live cell with two or three living neighbors lives on to the next generation.
- Any live cell with more than three living neighbors dies (overpopulation).
- Any dead cell with exactly three living neighbors becomes a living cell (reproduction).

## Features

- **Add/Remove Cells [Mouse Left].**
- **Load/Save Patterns [L/S].**\
  Pattern files are stored in folder <em>/patterns</em> using [Run Length Encoded](https://conwaylife.com/wiki/Run_Length_Encoded) format. Example of pattern files can be found at [https://conwaylife.com/wiki](https://conwaylife.com/wiki).
- **Populate [P].**\
  Fill the grid with a random pattern of living cells.
- **Reset [R].**\
  Reset active pattern to its initial configuration.
- **Rule [U].**\
  Edit automata rules using [Golly notation](https://en.wikipedia.org/wiki/Life-like_cellular_automaton#Notation_for_rules). Examples of life-like rules can be found at [https://conwaylife.com/wiki/List_of_Life-like_rules](https://conwaylife.com/wiki/List_of_Life-like_rules).
- **Clear Grid [G].**
- **Change Grid Size [Up/Down].**\
  Maximum grid size currently set to 960x490 cells.
- **Control Zoom [Mouse Wheel].**
- **Control Update Speed [Left/Right].**
- **Drag View [Mouse Right].**

## Build and Install

- Clone the repository to your local machine.
   ```bash
   git clone https://github.com/alejandrofsevilla/game-of-life.git
   cd game-of-life
- Build.
   ```bash
   cmake -S . -B build
   cmake --build build
- Portable installation.
   ```bash
   cmake --install build

## Requirements
* C++17 compiler.
* CMake 3.22.0.
* [SFML 2.6.0 requirements](https://www.sfml-dev.org/tutorials/2.6/start-cmake.php#requirements).
