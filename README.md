# Watopoly

## Description

Watopoly is a C++ implementation of a Monopoly-style board game themed around the University of Waterloo experience. The game features a dynamic 40-square board, text-based display, and gameplay mechanics faithful to the original Monopoly—adapted with UW-specific features. Players roll dice, buy and improve properties, pay tuition, and experience themed events such as “Goose Nesting” or “DC Tims Line,” all while managing their finances and attempting to avoid bankruptcy.

The project showcases strong object-oriented design with modular components and clean separation between game logic, user input, and display rendering. Key patterns used include the **Observer Pattern** for synchronizing display updates with game state, and the **Factory Method Pattern** for square initialization.

Collabrators: Hamza Mostafa Deven Kaler

---

## Features

* 🎲 **Gameplay Loop**: Supports rolling, trading, improving buildings, mortgaging, auctions, saving/loading, and winning logic.
* 🧱 **Board Construction**: Dynamically constructs a board of 40 specialized square types using a SquareFactory.
* 👥 **Multiple Players**: Supports up to 6 players, each with unique tokens and dynamic asset tracking.
* 🖼️ **Text-Based Display**: ASCII-rendered board with live updates using the Observer Pattern.
* 🏫 **Property System**: Distinct behaviors for Academic Buildings, Residences, and Gyms with rent logic and improvement rules.
* 💡 **Design Patterns**: Uses Observer, Factory, and Strategy patterns for extensibility and modularity.
* 💾 **Save/Load Support**: Save and resume games from a file.
* 🧪 **Testing Mode**: Play using rigged dice to test edge cases.

---

## Notes

* The `board.txt` file defines the ASCII layout and must be in the execution directory.
* Smart pointers (`shared_ptr`) are used throughout for safe memory management.
* Improvements are implemented via internal counters rather than decorators to simplify behavior and enforce building rules.

---

## Requirements

* **C++17 or later**
* **make** or other C++ build system
* **Standard C++ Libraries**
* Terminal emulator to view ASCII output

---

## Build and Run Instructions

```bash
# Compile
make watopoly

# Run new game
./watopoly

# Run game from save file
./watopoly <savefile.txt>

# Run in test mode with rigged dice
./watopoly -testing
```

---

## Usage

Once launched, the game accepts the following text commands:

* `roll`: Roll dice and move forward.
* `next`: End your turn.
* `improve <property> buy|sell`: Modify building improvements.
* `mortgage|unmortgage <property>`: Manage property mortgages.
* `trade <player> <give> <receive>`: Propose a trade.
* `save <filename>`: Save current game state.
* `assets`, `all`: View player and game stats.
* `bankrupt`: Declare bankruptcy.

---
