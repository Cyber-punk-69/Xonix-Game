# Contributing to Xonix Game

Thank you for considering contributing to the Xonix project! This document provides guidelines for contributing.

## How Can I Contribute?

### Reporting Bugs

Before creating bug reports, please check existing issues. When creating a bug report, include:

- **Description**: Clear description of the bug
- **Steps to Reproduce**: Detailed steps to reproduce the issue
- **Expected Behavior**: What you expected to happen
- **Actual Behavior**: What actually happened
- **Environment**: OS, compiler version, SFML version
- **Screenshots**: If applicable

**Example:**
```
**Bug**: Game crashes when loading save file

**Steps to Reproduce**:
1. Create a new game
2. Play for 5 minutes
3. Save the game
4. Exit and restart
5. Try to load the save

**Environment**: Ubuntu 22.04, g++ 11.3.0, SFML 2.5.1
```

### Suggesting Enhancements

Enhancement suggestions are tracked as GitHub issues. When creating an enhancement suggestion, include:

- **Clear title**: Summarize the enhancement
- **Detailed description**: Explain the feature and why it would be useful
- **Examples**: Provide examples of how it would work
- **Alternatives**: Describe alternatives you've considered

### Pull Requests

1. **Fork the repository**
2. **Create a branch** (`git checkout -b feature/AmazingFeature`)
3. **Make your changes**
4. **Test thoroughly**
5. **Commit** with clear messages
6. **Push** to your fork
7. **Open a Pull Request**

## Development Guidelines

### Code Style

- Use consistent indentation (2 or 4 spaces, no tabs)
- Follow existing naming conventions:
  - Classes: `PascalCase` (e.g., `PlayerState`, `ScoreSystem`)
  - Functions: `camelCase` (e.g., `updatePosition`, `checkCollision`)
  - Variables: `camelCase` (e.g., `playerX`, `enemyCount`)
  - Constants: `UPPER_SNAKE_CASE` (e.g., `MAX_ENEMIES`, `GRID_SIZE`)
- Add comments for complex logic
- Keep functions focused and small

### Commit Messages

Use clear, descriptive commit messages:

```
Good:
- "Add power-up freeze enemy functionality"
- "Fix collision detection bug in multiplayer mode"
- "Refactor leaderboard sorting algorithm"

Bad:
- "fix"
- "update"
- "changes"
```

### Testing

Before submitting:

1. **Compile without warnings**
   ```bash
   g++ -Wall -Wextra -c *.cpp
   ```

2. **Test all game modes**:
   - Single player
   - Multiplayer
   - Save/Load
   - Leaderboard
   - Friends system

3. **Check for memory leaks** (optional but recommended):
   ```bash
   valgrind --leak-check=full ./game
   ```

### Adding New Features

When adding new features:

1. **Create a new branch** for the feature
2. **Update documentation** (README.md, comments)
3. **Add example usage** if applicable
4. **Test edge cases**
5. **Update Group1_ProjectReport.md** if it's a major feature

### File Organization

- **Headers (.h)**: Class/function declarations, constants
- **Implementation (.cpp)**: Function definitions
- **Keep related code together**: Don't split small modules unnecessarily

## Project Structure

```
├── Core Game Logic
│   ├── main.cpp           # Main game loop
│   ├── Level.cpp/h        # Level management
│   └── Player.cpp/h       # Player mechanics
│
├── Systems
│   ├── Auth.cpp/h         # Authentication
│   ├── ScoreSystem.cpp/h  # Scoring
│   ├── Inventory.cpp/h    # Items/Power-ups
│   └── SaveGame.cpp/h     # Save/Load
│
├── Social Features
│   ├── Friends.cpp/h      # Friend system
│   ├── Leaderboard.cpp/h  # High scores
│   └── MatchMaking.cpp/h  # Matchmaking
│
└── Data Structures
    ├── AVLTree.cpp/h      # AVL tree
    ├── TileNode.h         # Linked list
    └── UserDB.cpp/h       # User database
```

## Areas We'd Love Help With

- [ ] Add more power-ups
- [ ] Implement online multiplayer (networking)
- [ ] Add sound effects and music
- [ ] Create more levels and themes
- [ ] Improve AI for enemies
- [ ] Add achievements system
- [ ] Mobile port (touch controls)
- [ ] Better graphics/animations
- [ ] Level editor
- [ ] Tutorial mode

## Questions?

Feel free to:
- Open an issue for discussion
- Ask in pull request comments
- Contact the maintainers

## Code of Conduct

- Be respectful and inclusive
- Provide constructive feedback
- Help others learn and grow
- Credit others' work appropriately

---

Thank you for contributing! 🎮
