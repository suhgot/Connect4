Connect 4 in C
A modular command-line implementation of Connect 4 written in C and developed/tested using a BeagleBoard embedded Linux platform. The project supports configurable square board sizes, human gameplay, two computer-player strategies, and automated performance comparisons between a baseline Slow Computer and an optimized Fast Computer.
The project was also part of my broader embedded/computer-architecture learning experience, where I gained exposure to MIPS architecture and assembly concepts, including low-level instruction execution, registers, memory access, and the relationship between software and processor hardware.
Features
Configurable `n x n` board sizes from 4 x 4 through 128 x 128
Standard Connect 4 win condition: 4 consecutive pieces
Three gameplay modes:
Human vs. Human
Human vs. Slow Computer
Fast Computer vs. Slow Computer
Input validation for board size, menu choices, and player moves
Horizontal, vertical, and diagonal win detection
Separate slow and optimized game-over algorithms
Heuristic computer move selection
Automated AI-vs-AI simulations
Runtime benchmarking using monotonic timing
Simulation summaries including win percentage and average compute time
Project Overview
The program separates the game into several modules so that board management, player input, shared game logic, and AI behavior remain independent.
The Slow Computer serves as a baseline implementation. It evaluates candidate moves using more expensive board-wide operations.
The Fast Computer improves move selection and win detection by examining only the lines that are relevant to a candidate or most recent move. It prioritizes:
Immediate winning moves
Blocking an opponent's immediate win
Extending strong local lines
Open-ended combinations
Center-column control
Local opponent threats
This makes the project useful not only as a playable Connect 4 game, but also as a small demonstration of algorithm optimization and runtime comparison in C.
Repository Structure
```text
Connect4/
├── main.c      # Program entry point, gameplay control, simulations, and timing
├── game.c      # Shared game helper functions
├── game.h      # Shared constants, enums, and Board structure
├── board.c     # Board creation, display, moves, and win detection
├── board.h     # Board function declarations
├── human.c     # User input and validation
├── human.h     # Human-input function declarations
├── ai.c        # Slow and Fast computer-player algorithms
└── ai.h        # AI function declarations
```
Hardware and Software Environment
This project was developed and tested using a BeagleBoard running Linux, providing hands-on experience compiling, executing, and benchmarking C programs on an embedded hardware platform.
Hardware
BeagleBoard embedded development board
ARM-based embedded Linux environment
Software
C11-compatible compiler
GCC
Linux command-line environment
The program uses `clock_gettime()` with `CLOCK_MONOTONIC` for performance measurements.
The source code can also be compiled on other Linux, macOS, or compatible Unix-like systems with an appropriate C compiler.
Build
Clone the repository:
```bash
git clone https://github.com/suhgot/Connect4.git
cd Connect4
```
Compile with GCC:
```bash
gcc -std=c11 -Wall -Wextra -O2 main.c board.c game.c human.c ai.c -o connect4
```
On some older Linux systems, `clock_gettime()` may require linking against `librt`:
```bash
gcc -std=c11 -Wall -Wextra -O2 main.c board.c game.c human.c ai.c -o connect4 -lrt
```
Run
Run the compiled executable:
```bash
./connect4
```
The program first asks for the board dimension:
```text
Welcome to Connect 4!
Enter n (Board size will be n x n):
```
Valid board sizes are:
```text
4 <= n <= 128
```
You can then select one of the available game modes:
```text
1. Human vs. Human
2. Human vs. Slow Computer
3. Fast Computer vs. Slow Computer
Select Choice (1-3):
```
Gameplay
The board uses:
`X` for Player X
`O` for Player O
`.` for an empty position
Players choose a column number. The piece falls into the lowest available row in that column, following normal Connect 4 gravity.
A player wins by connecting four pieces:
Horizontally
Vertically
Diagonally
If the board fills without either player connecting four pieces, the game ends in a tie.
Game Modes
1. Human vs. Human
Two human players alternate turns and enter column numbers from the terminal.
2. Human vs. Slow Computer
The human player plays as `X`, while the Slow Computer plays as `O`.
The Slow Computer:
Takes an immediate winning move when available
Blocks an opponent's immediate winning move
Otherwise attempts to extend its strongest existing line
Falls back to the first available legal column
3. Fast Computer vs. Slow Computer
The Fast Computer plays as `X` and the Slow Computer plays as `O`.
You can choose how many games to simulate.
For a single simulation, the program displays each move and board state.
For multiple simulations, detailed board output is suppressed and the program reports aggregate results instead.
Fast vs. Slow Algorithms
Slow Game-Over Check
The slow implementation scans the board for every possible horizontal, vertical, and diagonal Connect 4 sequence.
This is intentionally retained as a baseline for comparison.
Fast Game-Over Check
The optimized implementation uses the location of the most recent move.
Instead of searching the entire board, it checks only the:
Row containing the new piece
Column containing the new piece
Downward diagonal
Upward diagonal
Because any new win must include the most recently placed piece, unrelated areas of the board do not need to be searched.
Fast Computer Heuristic
The Fast Computer uses bounded local evaluation rather than repeated full-board scans.
Candidate moves are scored using factors such as:
Immediate wins
Opponent threat blocking
Three-in-a-row extensions
Two-in-a-row extensions
Open-ended lines
Center preference
The local directional scans stop once enough information has been collected to determine a Connect 4 line, avoiding unnecessary work on large boards.
Performance Benchmarking
In Fast Computer vs. Slow Computer mode, the program measures execution time for each AI turn using:
```c
clock_gettime(CLOCK_MONOTONIC, ...)
```
Timing includes:
Move selection
Piece placement
Game-over evaluation
Console printing is performed outside of the timed region so that terminal output does not distort the algorithm comparison.
When multiple games are simulated, the summary includes:
```text
Simulation Summary
------------------------------
Fast Computer win %: ...
Slow Computer win %: ...
Tie %: ...

Performance Metrics
Fast Computer average compute time per turn: ... ms
Slow Computer average compute time per turn: ... ms
Total # of turns played by Fast Computer: ...
Total # of turns played by Slow Computer: ...
Average # of turns per game: ...
```
This allows the optimized and baseline approaches to be compared directly as the board size or number of simulations increases.
Implementation Details
Board Representation
The board is stored in a fixed maximum-size two-dimensional array:
```c
char cell[128][128];
```
The active board dimensions are stored separately, allowing the program to use any valid `n x n` region between 4 and 128.
The project indexes board cells as:
```text
cell[column][row]
```
rather than the more common `cell[row][column]`.
Win Condition
The number of connected pieces required to win is defined as:
```c
#define NUM_WIN 4
```
Board Limits
```c
#define MIN_SIZE 4
#define MAX_SIZE 128
```
Example Workflow
```text
Welcome to Connect 4!
Enter n (Board size will be n x n): 7

1. Human vs. Human
2. Human vs. Slow Computer
3. Fast Computer vs. Slow Computer
Select Choice (1-3): 2
```
The board is then displayed with numbered columns, and play continues until `X`, `O`, or a tie is detected.
Possible Future Improvements
Potential extensions include:
Standard 7 x 6 Connect 4 board option
Human vs. Fast Computer mode
Minimax or alpha-beta pruning
Adjustable AI difficulty
Randomized tie-breaking between equally scored moves
Test suite for board and AI functions
Makefile or CMake build configuration
Graphical user interface
Saving benchmark results to CSV
Additional profiling and complexity analysis
Technologies and Concepts
C
BeagleBoard
Embedded Linux
GCC
Modular programming
Header/source separation
Structs and enums
Two-dimensional arrays
Input validation
Heuristic algorithms
Local search optimization
Algorithmic complexity
Runtime benchmarking
Game-state simulation
Computer architecture
MIPS architecture and assembly fundamentals
Registers, instructions, and memory operations
Hardware/software interaction
Learning Outcomes
Through this project and the surrounding coursework, I gained practical experience with both software development and computer architecture.
Key areas included:
Writing and organizing a multi-file C program
Compiling and running software directly on a BeagleBoard
Working within an embedded Linux environment
Comparing baseline and optimized algorithms through measured runtime
Understanding how implementation choices affect execution performance
Applying modular programming techniques using `.c` and `.h` files
Strengthening my understanding of processor architecture through MIPS
Learning how registers, instructions, memory, and control flow operate at a lower level
Connecting high-level C programming concepts with underlying computer hardware behavior
Author
GitHub: @suhgot
Repository
github.com/suhgot/Connect4
