# Maze Game 2D

A console-based 2D maze game written in C++ for Windows. Navigate a procedurally generated maze, avoid traps and enemies with AI-driven behavior, collect the treasure, and try to beat your best score.

## Features

- **Procedurally generated maze** – A guaranteed solvable path is carved first, then extra open cells are added randomly for varied layouts each run.
- **Enemy AI via Decision Tree** – Enemies choose between `CHASE`, `GUARD`, and `PATROL` behaviors based on their distance to the player and the treasure.
- **BFS Pathfinding Hint** – Press `H` at any time to get the shortest path to the treasure, computed live with Breadth-First Search.
- **Undo System** – Made a wrong move? Press `U` to undo your last step, backed by a stack of previous positions.
- **Traps** – Hidden traps (`X`) scattered across the maze cost score points when stepped on, and disappear once triggered. Traps can also eliminate enemies that wander into them.
- **Score Tracking & Game History** – Every game result (win, lose, or quit) is logged with a timestamp and final score to `game_history.txt`.
- **Replayability** – Play again instantly after each round with a fresh maze layout.

## Controls

| Key         | Action              |
|-------------|---------------------|
| Arrow Keys  | Move the player     |
| `H`         | Show BFS shortest-path hint |
| `U`         | Undo last move       |
| `D`         | Show enemy AI decision tree |
| `Q`         | Quit the game         |

## Legend

| Symbol | Meaning   |
|--------|-----------|
| `P`    | Player    |
| `E`    | Enemy     |
| `X`    | Trap      |
| `T`    | Treasure  |
| `#`    | Wall      |

## How the Enemy AI Works

Each enemy evaluates a decision tree every turn based on its distance to the player and to the treasure:

```
                Player Near?
                /          \
              YES           NO
              /              \
     Treasure Far?       Near Treasure?
       /     \             /       \
     YES     NO          YES        NO
      |       |           |          |
    CHASE    GUARD       GUARD     PATROL
```

- **CHASE** – Moves directly toward the player.
- **GUARD** – Moves toward the treasure to defend it.
- **PATROL** – Moves in a random direction.

## Building & Running

This project uses the Windows-only `<conio.h>` and `<windows.h>` headers, so it requires a Windows environment with a C++ compiler (MinGW, MSVC, etc.).

### Using g++ (MinGW):

```bash
g++ -o maze_game maze_game.cpp
./maze_game
```

### Using Visual Studio:

1. Create a new C++ Console App project.
2. Replace the generated source file with `maze_game.cpp`.
3. Build and run (Ctrl+F5).

## Requirements

- Windows OS (due to `conio.h` / `windows.h` dependency)
- A C++ compiler supporting C++11 or later

## Project Structure

```
Maze-game-2D/
├── maze_game.cpp      # Main game source code
├── game_history.txt   # Auto-generated game log (created on first run)
└── README.md
```

## Possible Future Improvements

- Cross-platform support (replace `conio.h`/`windows.h` with a portable input library)
- Difficulty levels and configurable maze size
- Multiple treasures and level progression
- Persistent high-score leaderboard

## License

This project currently has no license specified. Feel free to add one (e.g., MIT) if you plan to share or accept contributions.