![LinuxWorkflow](https://github.com/alejandrofsevilla/game-of-life/actions/workflows/Linux.yml/badge.svg) ![WindowsWorkflow](https://github.com/alejandrofsevilla/game-of-life/actions/workflows/Windows.yml/badge.svg)
# Game of Life

A visual representation of [Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life), a cellular automata devised by the British mathematician John Horton Conway.

![ezgif-4-ffe8b51cf1](https://github.com/alejandrofsevilla/game-of-life/assets/110661590/2f0afaf2-a0fe-45b1-b5d5-be2378734f2b)

## About the Game of Life

Conway's Game of Life is a zero-player game that evolves based on its initial state. The rules are simple yet give rise to complex patterns. Cells in a grid live, die, or multiply based on the following rules:
1. Any live cell with fewer than two living neighbors dies (underpopulation).
2. Any live cell with two or three living neighbors lives on to the next generation.
3. Any live cell with more than three living neighbors dies (overpopulation).
4. Any dead cell with exactly three living neighbors becomes a living cell (reproduction).

## Features

- **Add/Remove Cells [Mouse Left]:** Click on cells to toggle between alive and dead states.
- **Load/Save Files[L/S]:** Patterns are stored in folder "/patterns" using [Run Length Encoded](https://conwaylife.com/wiki/Run_Length_Encoded) format. Examples can be found at [https://conwaylife.com/wiki](https://conwaylife.com/wiki).
- **Randomize Grid [G]:** Fill the grid with a random pattern of living cells.
- **Reset [R]:** Reset grid to its original pattern.
- **Clear Grid [C]:** Removes all cells from grid.
- **Change Grid Size [↑/↓]:** Maximum grid size currently set to 960x490 cells.
- **Control Zoom [Mouse Wheel].**
- **Control Simulation Speed [←/→].**
- **Control Camara [Mouse Right].**

## Build and Install

1. Clone the repository to your local machine.
   ```bash
   git clone https://github.com/alejandrofsevilla/game-of-life.git
   cd game-of-life
2. Build.
   ```bash
   cmake -S . -B build
   cmake --build build
3. Portable installation.
   ```bash
   cmake --install build

## Requirements
* C++17 compiler.
* CMake 3.22.0.
* [SFML 2.6.0 requirements](https://www.sfml-dev.org/tutorials/2.6/start-cmake.php#requirements).
