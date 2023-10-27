# ![LinuxWorkflow](https://github.com/alejandrofsevilla/game-of-life/actions/workflows/Linux.yml/badge.svg) ![WindowsWorkflow](https://github.com/alejandrofsevilla/game-of-life/actions/workflows/Windows.yml/badge.svg)
# Game of Life

A visual representation of [Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life), a cellular automata devised by the British mathematician John Horton Conway.

![ezgif-4-ffe8b51cf1](https://github.com/alejandrofsevilla/game-of-life/assets/110661590/2f0afaf2-a0fe-45b1-b5d5-be2378734f2b)

## About the Game of Life

Conway's Game of Life is a zero-player game that evolves based on its initial state. The rules are simple yet give rise to complex patterns. Cells in a grid live, die, or multiply based on the following rules:
- Any live cell with fewer than two living neighbors dies (underpopulation).
- Any live cell with two or three living neighbors lives on to the next generation.
- Any live cell with more than three living neighbors dies (overpopulation).
- Any dead cell with exactly three living neighbors becomes a living cell (reproduction).

## Features

- **Add/Remove Cells [Mouse Left].**
- **Load/Save Patterns [L/S].**\
  Pattern files are stored in folder <em>/patterns</em> using [Run Length Encoded](https://conwaylife.com/wiki/Run_Length_Encoded) format. Example of pattern files can be found at [https://conwaylife.com/wiki](https://conwaylife.com/wiki).
- **Generate Population [G].**\
  Fill the grid with a random pattern of living cells.
- **Reset [R].**\
  Reset active pattern to its initial configuration.
- **Clear Grid [G].**\
  Removes all living and dead cells from grid.
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
