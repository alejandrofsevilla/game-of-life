# ![LinuxWorkflow](https://github.com/alejandrofsevilla/game-of-life/actions/workflows/Linux.yml/badge.svg) ![WindowsWorkflow](https://github.com/alejandrofsevilla/game-of-life/actions/workflows/Windows.yml/badge.svg)
# Game of Life

Visualization of [Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) and other [life-like cellular automata](https://en.wikipedia.org/wiki/Life-like_cellular_automaton) using [SFML](https://www.sfml-dev.org/).

![Base Profile Screenshot 2024 10 21 - 23 24 49 33](https://github.com/user-attachments/assets/6b9d386e-eb8f-4a41-ad91-a64469da2ba1)

## About the Game of Life
Conway's Game is a cellular automata invented by the British mathematician John Horton Conway in 1970. It is a zero-player game that evolves based on its initial state. Cells in a grid live, die, or multiply based on the following rules:
- Any live cell with fewer than two living neighbors dies (underpopulation).
- Any live cell with two or three living neighbors lives on to the next generation.
- Any live cell with more than three living neighbors dies (overpopulation).
- Any dead cell with exactly three living neighbors becomes a living cell (reproduction).
  
Other Cellular automata are considered Life-like, when they present a similar behaviour to Conway's Game of Life and meet the following criteria:
- The array of cells of the automaton has two dimensions.
- Each cell of the automaton has two states.
- The neighborhood of each cell is the [Moore neighborhood](https://en.wikipedia.org/wiki/Moore_neighborhood).
- In each time step of the automaton, the new state of a cell can be expressed as a function of the number of adjacent cells that are in the alive state and of the cell's own state.
There are in total 2^18 possible life-like rules that can be studied.
## Requirements
* C++17 compiler.
* CMake 3.22.0.
* [SFML 2.6.0 requirements](https://www.sfml-dev.org/tutorials/2.6/start-cmake.php#requirements). 
## Features
- **Add/Remove Cells [Mouse Left].**
- **Load/Save Patterns [L/S].**\
  Pattern files are stored in folder <em>/patterns</em> using [Run Length Encoded](https://conwaylife.com/wiki/Run_Length_Encoded) format. Example of pattern files can be found at [https://conwaylife.com/wiki](https://conwaylife.com/wiki).
- **Populate [P].**\
  Fill the grid with a random pattern of living cells.
- **Reset [R].**\
  Reset active pattern to its initial configuration.
- **Rule [U].**\
  Edit automata rules using [Golly notation](https://en.wikipedia.org/wiki/Life-like_cellular_automaton#Notation_for_rules). The default value corresponds to that of the Conway´s Game Of Life, denoted B3/S23. A list other of life-like rules can be found at [https://conwaylife.com/wiki/List_of_Life-like_rules](https://conwaylife.com/wiki/List_of_Life-like_rules).
- **Clear Grid [G].**
- **Set Grid Size [Up/Down].**
- **Set Zoom Level [Mouse Wheel].**
- **Set Update Speed [Left/Right].**
- **Drag View [Mouse Right].**
## Build and Install
- Clone the repository to your local machine.
   ```terminal
   git clone https://github.com/alejandrofsevilla/game-of-life.git
   cd game-of-life
   ```
- Build.
   ```terminal
   cmake -S . -B build
   cmake --build build
   ```
- Portable installation.
   ```terminal
   cmake --install build
   ```
