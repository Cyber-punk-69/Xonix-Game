Group 1 — Xonix Project Report
=================================

Cover Page
----------

Project Title: Xonix — Tile Capture Arcade

Group Number: 1

Members:
- Member 1: NAME_1 (Roll: ROLL_1)
- Member 2: NAME_2 (Roll: ROLL_2)

Course: Data Structures & Algorithms (Section: ___)

Instructor: Dr. [Instructor Name]

Submission Date: [DD-MM-YYYY]


Introduction
------------

This document summarizes the design and implementation of our Xonix-style arcade game implemented in C++ using SFML for graphics and a set of small, file-backed modules for authentication, leaderboard, matchmaking and save/load features. The project goal was to implement a playable single-player and local multiplayer experience, include persistent user accounts and leaderboard, and demonstrate use of multiple data structures for game state, persistence and social features.

Key goals and implemented features
- Core Xonix gameplay: player draws trails to capture area while avoiding enemies.
- Menu system (Start, Multiplayer, Matchmaking, Select Level, Friends, Leaderboard, Save/Load, Logout, Exit).
- Authentication (register/login) with a file-backed user store.
- Persistent leaderboard and per-user inventory (power-ups) saved to plain text files.
- Local multiplayer (two players on same keyboard) and a simple matchmaking demo.
- Save/Load system that records filled tiles and player state.
- Friend system with requests, accept/reject and basic friend lists.
- Simple HUD, pause menu, and in-game power-up usage.

Tools and languages used
- Language: C++ (C++11-compatible style)
- Libraries: SFML (Graphics + Window), C standard I/O for persistence
- Build: Makefile or your preferred C++ build system
- Auxiliary: pandoc (optional) to convert this Markdown report to PDF


Work Distribution
-----------------

Summary of feature/module distribution between two members. Replace names/rolls above.

| Module / Task                     | Member 1 (NAME_1)     | Member 2 (NAME_2)     |
|----------------------------------:|:---------------------:|:---------------------:|
| Core game loop & player mechanics | Lead implementation   | Support / testing     |
| Enemy AI & collision detection   | Implement & tune      | Unit testing & debug  |
| SFML menu & UI                   | Layout & draw code    | Input handling & UX   |
| Authentication & persistence     | Design file format    | Implement read/write  |
| Leaderboard & Save/Load          | Serialization logic   | File IO & integration |
| Multiplayer & Matchmaking        | Local match logic     | Matchmaking engine    |
| Friends system                   | Data model            | UI integration        |
| Report & documentation           | Drafting              | Review & editing      |


Workflow & Implementation Timeline
----------------------------------

Planned milestones and actual development timeline (example):

1. Project setup & basic SFML window — Day 1
2. Implement playfield grid and player movement — Days 2–3
3. Enemy struct and collision detection — Days 3–4
4. Trail capture algorithm (flood-fill/drop) & scoring — Days 4–6
5. Menu system, pause, HUD — Days 6–7
6. Authentication / Leaderboard / SaveLoad modules (file-based) — Days 7–9
7. Multiplayer & Matchmaking demo — Days 9–11
8. Friends system & UI — Day 12
9. Polish, tests and final debugging — Days 13–14

(Optional) Use a simple Gantt or milestone list if desired. We recommend exporting the above timeline into a Gantt chart using Excel/Google Sheets when preparing the final PDF.


Data Structures Used
--------------------

Below we list the major data structures used in the project, where they are used in the code and why they were chosen.

1) Static 2D array for playfield (int grid[M][N])
- Usage: main playfield storing tiles: 0 empty, 1 filled, 2 player trail, 3 other trail markers, -1 temporary visited marker.
- Reason: Fixed small board size (25x40) makes a static 2D array efficient and easy to index by tile coordinates. Constant-time access simplifies collision and rendering.

2) Plain struct arrays for enemies and players (Enemy enemies[20], PlayerState p1/p2)
- Usage: Store enemy positions, velocities and player states.
- Reason: Small fixed upper bound on enemy count allows compact contiguous storage and fast iteration. Simple struct provides clear fields for movement and rendering.

3) Linked list for save tiles (TileNode)
- Usage: SaveGame collects filled tile coordinates into a linked list when saving and reads them back on load.
- Reason: The number of saved tiles can vary; using a singly linked list avoids dynamic array resizing boilerplate and matches a streaming append use-case during save traversal.

4) Simple flat text files for persistence (users.txt, leaderboard.txt, powerups.txt, friends.txt)
- Usage: Persist user accounts, leaderboard entries, per-user inventory and friend relationships.
- Reason: Simplicity and portability; data is small and human-readable for debugging. Operations performed are mostly append or full-file rewrite so no advanced index needed.

5) Matchmaking queue (simple array / ring buffer)
- Usage: Enqueue players (or bots), perform pairing operations to produce GameRoom results.
- Reason: Matching scale is small and operations are queue-like, so arrays with head/tail indices are sufficient.

6) Friend lists and pending requests (fixed-size arrays inside FriendSystem)
- Usage: Store players and friend links using arrays and counters.
- Reason: Implementation simplicity and predictable memory usage in the assignment context.

Justification notes
- We prioritized simplicity and deterministic memory usage since the project scope fits classroom constraints.
- For production-scale uses or large datasets, one would replace flat files with a small database, replace linked lists with dynamic arrays or sparse bitsets for faster random access, and use proper concurrency controls for matchmaking.


Challenges Faced & Solutions
---------------------------

1) Challenge: Reliable area-capture computation (ensuring correct flood-fill and enemy reachability)
- Problem: When the player closes a loop, the game must determine which area is cut off from enemies and convert those tiles to filled state without accidentally filling areas reachable by enemies.
- Solution: We implemented a "drop" flood function that marks reachable tiles from each enemy location with a temporary marker (-1). After marking, we convert unmarked zeros into filled tiles (1) and revert -1 back to 0. This approach reliably separates reachable vs captured areas.

2) Challenge: Consistent user input & non-blocking modal dialogs in SFML
- Problem: Implementing modal text entry (login, name entry for high score, save IDs) while keeping the window responsive, and without resorting to blocking console IO.
- Solution: A custom on-screen text input routine (showTextInput) processes events and draws an input box with blinking cursor. It handles Return/Escape/Backspace/TextEntered events and returns the typed string.

3) Challenge: Simple persistence and cross-module integration
- Problem: Multiple features needed persistent storage (users, friends, leaderboard, saves) but a full DB is out of scope.
- Solution: We used plain text file formats that are easy to parse, make human-readable debugging possible, and provide atomic-like semantics by reading whole files into memory and rewriting them when updating entries.


Screenshots & Sample Outputs
---------------------------

Please include screenshots from the running binary to illustrate these screens (place images in the project's images/ or docs/ folder):
- Main menu (title, options list)
- Authentication dialog (login / register)
- In-game HUD showing score and power-ups
- End screen showing score and high-score name entry
- Local multiplayer screen with two players
- Matchmaking queue UI and recent matches
- Friends UI showing list, requests and send/search flow

(Example image filenames to include before exporting to PDF):
- images/menu_main.png
- images/auth_screen.png
- images/in_game_hud.png
- images/gameover_highscore.png
- images/local_multiplayer.png
- images/matchmaking.png
- images/friends_ui.png

Note: Replace placeholders above with actual screenshots captured when running the game. Use the following pandoc command to export this Markdown to PDF (requires pandoc + LaTeX):

pandoc "Group1_ProjectReport.md" -o "Group1_ProjectReport.pdf"


Conclusion
----------

Learning outcomes:
- Applied multiple fundamental data structures (2D arrays, linked lists, queues) to a small game project and reasoned about their strengths and trade-offs.
- Practiced event-driven UI programming with SFML and implemented robust text input and modal flows.
- Implemented file-based persistence for accounts, leaderboard, save/load and friends systems.

Potential improvements and future features:
- Replace file-based storage with a lightweight database (SQLite) to support scalable persistence and transactions.
- Networked multiplayer with real sockets and authoritative server to enable remote matches.
- Improved AI (enemy pathfinding), additional power-ups, and more levels with varied objectives.
- Unit tests and automated integration tests for IO modules.


Appendix
--------

References:
- SFML documentation: https://www.sfml-dev.org/
- Flood-fill and capture algorithms: standard graph traversal resources

Selected pseudocode snippet (capture algorithm concept):

1. When player closes a loop (returns to a filled tile):
   a. For each enemy, run flood-fill from enemy tile and mark reachable tiles as TEMP.
   b. Any tile not marked TEMP and currently empty or trail becomes FILLED.
   c. Revert TEMP marks back to EMPTY.

2. Update score based on number of newly filled tiles and elapsed time.


End of report


(Replace NAME_1, NAME_2, ROLL_1, ROLL_2 and instructor/date placeholders with actual values before final submission.)
