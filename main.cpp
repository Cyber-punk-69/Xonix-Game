#include "Auth.h"
#include "Friends.h"
#include "Leaderboard.h"
#include "Level.h"
#include "MatchMaking.h"
#include "Multiplayer.h"
#include "ScoreSystem.h"
#include <SFML/Graphics.hpp>

#include <cstdint>
#include <iostream>
#include <string>
#include <time.h>

#include "Inventory.h"
#include "SaveGame.h"

using namespace sf;
using namespace std;

const int M = 25;
const int N = 40;
int grid[M][N] = {0};
int ts = 18;

struct Enemy
{
  int x, y, dx, dy;
  Enemy()
  {
    x = y = 300;
    dx = 4 - rand() % 8;
    dy = 4 - rand() % 8;
  }
  void move()
  {
    x += dx;
    if (grid[y / ts][x / ts] == 1)
    {
      dx = -dx;
      x += dx;
    }
    y += dy;
    if (grid[y / ts][x / ts] == 1)
    {
      dy = -dy;
      y += dy;
    }
  }
};

void drop(int y, int x)
{
  if (y < 0 || y >= M || x < 0 || x >= N)
    return;
  if (grid[y][x] == 0)
    grid[y][x] = -1;
  if (y - 1 >= 0 && grid[y - 1][x] == 0)
    drop(y - 1, x);
  if (y + 1 < M && grid[y + 1][x] == 0)
    drop(y + 1, x);
  if (x - 1 >= 0 && grid[y][x - 1] == 0)
    drop(y, x - 1);
  if (x + 1 < N && grid[y][x + 1] == 0)
    drop(y, x + 1);
}

void drawMenuText(RenderWindow &win, Font &font, const string &str, int size,
                  float x, float y, bool bold = false,
                  Color textColor = Color::White)
{
  Text t;
  t.setFont(font);
  t.setString(str);
  t.setCharacterSize(size);
  t.setPosition(x, y);
  t.setFillColor(textColor);
  if (bold)
    t.setStyle(Text::Bold);
  win.draw(t);
}

// Parse RGB color from string like "RGB(255,0,0)" or return default
Color parseColorFromTheme(const string &colorCode,
                          Color defaultColor = Color(135, 206, 250))
{
  if (colorCode.empty())
    return defaultColor;

  // Try to parse RGB format: "RGB(r,g,b)"
  int r = 0, g = 0, b = 0;
  if (sscanf(colorCode.c_str(), "RGB(%d,%d,%d)", &r, &g, &b) == 3)
  {
    return Color(r, g, b);
  }

  return defaultColor;
}

// Get background color from theme
Color getThemeBackgroundColor(const Theme &theme,
                              Color defaultColor = Color(135, 206, 250))
{
  if (!theme.colorCode.empty())
  {
    return parseColorFromTheme(theme.colorCode, defaultColor);
  }
  return defaultColor;
}

string showTextInput(RenderWindow &window, Font &font, const string &prompt,
                     const string &initial = "", int maxLen = 24)
{
  string text = initial;
  Clock blink;
  while (window.isOpen())
  {
    Event ev;
    while (window.pollEvent(ev))
    {
      if (ev.type == Event::Closed)
      {
        window.close();
        return text;
      }
      if (ev.type == Event::KeyPressed)
      {
        if (ev.key.code == Keyboard::Return)
        {
          return text;
        }
        if (ev.key.code == Keyboard::Escape)
        {
          return string();
        }
        if (ev.key.code == Keyboard::BackSpace)
        {
          if (!text.empty())
            text.pop_back();
        }
      }
      if (ev.type == Event::TextEntered)
      {
        uint32_t c = ev.text.unicode;
        if (c >= 32 && c <= 126 && (int)text.size() < maxLen)
        {
          // avoid spaces if requested? keep as-is, caller will sanitize
          text.push_back((char)c);
        }
      }
    }

    // Firewatch-style dialog
    Color dialogBgColor = Color(135, 206, 250); // Default sky blue
    window.clear(dialogBgColor);

    // dim background by drawing a translucent rect
    RectangleShape dim(
        Vector2f((float)window.getSize().x, (float)window.getSize().y));
    dim.setFillColor(Color(0, 0, 0, 120));
    window.draw(dim);

    // dialog box - clean and minimal
    float w = 520.f, h = 140.f;
    RectangleShape box(Vector2f(w, h));
    box.setPosition((window.getSize().x - w) / 2.f,
                    (window.getSize().y - h) / 2.f);
    box.setFillColor(Color(255, 255, 255, 240));
    box.setOutlineColor(Color(200, 200, 200));
    box.setOutlineThickness(2.f);
    window.draw(box);

    // prompt
    drawMenuText(window, font, prompt, 20, box.getPosition().x + 18.f,
                 box.getPosition().y + 12.f, true, Color(50, 50, 50));

    // input field background
    RectangleShape field(Vector2f(w - 36.f, 40.f));
    field.setPosition(box.getPosition().x + 18.f, box.getPosition().y + 56.f);
    field.setFillColor(Color(250, 250, 250));
    field.setOutlineColor(Color(180, 180, 180));
    field.setOutlineThickness(1.f);
    window.draw(field);

    // text with blinking cursor
    bool showCursor = ((int)(blink.getElapsedTime().asSeconds() * 2) % 2) == 0;
    string display = text + (showCursor ? "|" : "");
    drawMenuText(window, font, display, 20, field.getPosition().x + 8.f,
                 field.getPosition().y + 6.f, false, Color(30, 30, 30));

    // hint
    drawMenuText(window, font, "Enter=OK  Esc=Cancel", 14,
                 box.getPosition().x + 18.f, box.getPosition().y + h - 28.f,
                 false, Color(100, 100, 100));

    window.display();
  }
  return text;
}

bool showAuthScreen(RenderWindow &window, Font &font, Auth &auth,
                    const Player *&loggedPlayer)
{
  int choice = 0;
  const int CHOICES = 3;
  string opts[CHOICES] = {"Login", "Register", "Continue as Guest"};

  bool inSelection = true;
  bool inInput = false;
  bool registerMode = false;
  string fieldUsername = "";
  string fieldPassword = "";
  string fieldNickname = "";

  int focus = 0;

  while (window.isOpen())
  {
    Event ev;
    while (window.pollEvent(ev))
    {
      if (ev.type == Event::Closed)
      {
        window.close();
        return true;
      }

      if (inSelection)
      {
        if (ev.type == Event::KeyPressed)
        {
          if (ev.key.code == Keyboard::Up)
            choice = (choice - 1 + CHOICES) % CHOICES;
          if (ev.key.code == Keyboard::Down)
            choice = (choice + 1) % CHOICES;
          if (ev.key.code == Keyboard::Return)
          {
            if (choice == 0)
            { // Login
              inSelection = false;
              inInput = true;
              registerMode = false;
              fieldUsername = "";
              fieldPassword = "";
              fieldNickname = "";
              focus = 0;
            }
            else if (choice == 1)
            { // Register
              inSelection = false;
              inInput = true;
              registerMode = true;
              fieldUsername = "";
              fieldPassword = "";
              fieldNickname = "";
              focus = 0;
            }
            else
            {                         // Guest
              loggedPlayer = nullptr; // guest
              return true;
            }
          }
        }
      }
      else if (inInput)
      {
        if (ev.type == Event::KeyPressed)
        {
          if (ev.key.code == Keyboard::Tab)
          {
            if (registerMode)
              focus = (focus + 1) % 3;
            else
              focus = (focus + 1) % 2;
          }
          else if (ev.key.code == Keyboard::Escape)
          {
            inInput = false;
            inSelection = true;
          }
          else if (ev.key.code == Keyboard::Return)
          {
            if (!registerMode)
            {
              const Player *p = auth.loginUser(fieldUsername, fieldPassword);
              if (p != nullptr)
              {
                loggedPlayer = p;
                return true;
              }
              else
              {
                fieldPassword = "";
              }
            }
            else
            {
              bool ok = auth.registerUser(fieldUsername, fieldPassword,
                                          fieldNickname);
              if (ok)
              {
                const Player *p = auth.loginUser(fieldUsername, fieldPassword);
                loggedPlayer = p;
                return true;
              }
              else
              {
                fieldPassword = "";
              }
            }
          }
          else if (ev.key.code == Keyboard::BackSpace)
          {
            if (focus == 0 && !fieldUsername.empty())
              fieldUsername.pop_back();
            else if (focus == 1 && !fieldPassword.empty())
              fieldPassword.pop_back();
            else if (focus == 2 && !fieldNickname.empty())
              fieldNickname.pop_back();
          }
        }
        // text input (letters, numbers, punctuation)
        if (ev.type == Event::TextEntered)
        {
          uint32_t c = ev.text.unicode;
          if (c >= 32 && c <= 126)
          { // printable ASCII
            char ch = static_cast<char>(c);
            if (focus == 0)
              fieldUsername.push_back(ch);
            else if (focus == 1)
              fieldPassword.push_back(ch);
            else if (focus == 2)
              fieldNickname.push_back(ch);
          }
        }
      }
    }

    // Modern dark theme for login screen
    window.clear(Color(25, 30, 40));

    // Title with better styling
    RectangleShape titleBar(Vector2f(window.getSize().x, 80.f));
    titleBar.setPosition(0, 0);
    titleBar.setFillColor(Color(40, 50, 70));
    window.draw(titleBar);
    drawMenuText(window, font, "XONIX", 48, window.getSize().x / 2.f - 80.f,
                 15.f, true, Color::White);

    if (inSelection)
    {
      float centerX = window.getSize().x / 2.f - 100.f;
      float startY = 150.f;
      drawMenuText(window, font, "Please select:", 22, centerX, startY, false,
                   Color(220, 220, 220));

      for (int i = 0; i < CHOICES; ++i)
      {
        bool sel = (i == choice);
        Color itemColor = sel ? Color(255, 200, 50) : Color::White;
        int fontSize = sel ? 24 : 20;
        drawMenuText(window, font, opts[i], fontSize, centerX,
                     startY + 60.f + i * 45.f, sel, itemColor);
      }
      drawMenuText(window, font, "Use Up/Down + Enter", 14, centerX,
                   startY + 60.f + CHOICES * 45.f + 20.f, false,
                   Color(180, 180, 180));
    }
    else if (inInput)
    {
      float centerX = window.getSize().x / 2.f - 150.f;
      float startY = 140.f;

      if (!registerMode)
      {
        drawMenuText(window, font, "Login", 32, centerX, startY, true,
                     Color::White);

        // Input fields with better contrast
        drawMenuText(window, font, "Username:", 18, centerX, startY + 60.f,
                     false, Color(220, 220, 220));
        Color fieldColor = focus == 0 ? Color(255, 200, 50) : Color::White;
        string userDisplay = fieldUsername.empty() ? "_" : fieldUsername;
        drawMenuText(window, font, userDisplay, 20, centerX + 150.f,
                     startY + 60.f, false, fieldColor);

        drawMenuText(window, font, "Password:", 18, centerX, startY + 100.f,
                     false, Color(220, 220, 220));
        string masked = "";
        for (size_t i = 0; i < fieldPassword.size(); ++i)
          masked.push_back('*');
        if (masked.empty())
          masked = "_";
        fieldColor = focus == 1 ? Color(255, 200, 50) : Color::White;
        drawMenuText(window, font, masked, 20, centerX + 150.f, startY + 100.f,
                     false, fieldColor);

        drawMenuText(window, font,
                     "TAB to switch, ENTER to submit, ESC to cancel", 14,
                     centerX, startY + 160.f, false, Color(180, 180, 180));
      }
      else
      {
        drawMenuText(window, font, "Register", 32, centerX, startY, true,
                     Color::White);

        drawMenuText(window, font, "Username (>=3 chars):", 18, centerX,
                     startY + 60.f, false, Color(220, 220, 220));
        Color fieldColor = focus == 0 ? Color(255, 200, 50) : Color::White;
        string userDisplay = fieldUsername.empty() ? "_" : fieldUsername;
        drawMenuText(window, font, userDisplay, 20, centerX + 220.f,
                     startY + 60.f, false, fieldColor);

        drawMenuText(window, font, "Password (>=4 chars):", 18, centerX,
                     startY + 100.f, false, Color(220, 220, 220));
        string masked = "";
        for (size_t i = 0; i < fieldPassword.size(); ++i)
          masked.push_back('*');
        if (masked.empty())
          masked = "_";
        fieldColor = focus == 1 ? Color(255, 200, 50) : Color::White;
        drawMenuText(window, font, masked, 20, centerX + 220.f, startY + 100.f,
                     false, fieldColor);

        drawMenuText(window, font, "Nickname (optional):", 18, centerX,
                     startY + 140.f, false, Color(220, 220, 220));
        fieldColor = focus == 2 ? Color(255, 200, 50) : Color::White;
        string nickDisplay = fieldNickname.empty() ? "_" : fieldNickname;
        drawMenuText(window, font, nickDisplay, 20, centerX + 220.f,
                     startY + 140.f, false, fieldColor);

        drawMenuText(window, font,
                     "TAB to move, ENTER to register, ESC to cancel", 14,
                     centerX, startY + 200.f, false, Color(180, 180, 180));
      }
    }

    window.display();
  }

  return true;
}

bool loadUserInventory(const string &username, int &outInv)
{
  outInv = 0;
  FILE *f = fopen("powerups.txt", "r");
  if (!f)
    return false;
  char buf[256];
  while (fgets(buf, sizeof(buf), f))
  {
    char u[128];
    int inv;
    if (sscanf(buf, "%127s %d", u, &inv) == 2)
    {
      if (username == string(u))
      {
        outInv = inv;
        fclose(f);
        return true;
      }
    }
  }
  fclose(f);
  return false;
}

void saveUserInventory(const string &username, int inv)
{
  const int MAX_LINES = 1024;
  char lines[MAX_LINES][256];
  int count = 0;
  FILE *f = fopen("powerups.txt", "r");
  if (f)
  {
    while (count < MAX_LINES && fgets(lines[count], sizeof(lines[count]), f))
      count++;
    fclose(f);
  }
  bool updated = false;
  f = fopen("powerups.txt", "w");
  if (!f)
    return;
  for (int i = 0; i < count; ++i)
  {
    char u[128];
    int v;
    if (sscanf(lines[i], "%127s %d", u, &v) == 2)
    {
      if (username == string(u))
      {
        fprintf(f, "%s %d\n", u, inv);
        updated = true;
      }
      else
      {
        fprintf(f, "%s %d\n", u, v);
      }
    }
  }
  if (!updated)
  {
    fprintf(f, "%s %d\n", username.c_str(), inv);
  }
  fclose(f);
}

int main()
{
  srand((unsigned int)time(0));

  RenderWindow window(VideoMode(N * ts, M * ts), "Xonix with Menu (SFML)");
  window.setFramerateLimit(60);

  Font font;
  if (!font.loadFromFile("fonts/arial.ttf"))
  {
    cerr << "Warning: fonts/arial.ttf not found. Menu text may not render as "
            "expected.\n";
  }

  Texture tTile, tEnemy, tGameover;
  if (!tTile.loadFromFile("images/tiles.png"))
  {
    cerr << "Warning: images/tiles.png not found.\n";
  }
  if (!tEnemy.loadFromFile("images/enemy.png"))
  {
    cerr << "Warning: images/enemy.png not found.\n";
  }
  if (!tGameover.loadFromFile("images/gameover.png"))
  {
    cerr << "Warning: images/gameover.png not found.\n";
  }

  Sprite sTile(tTile), sEnemy(tEnemy), sGameover(tGameover);
  sEnemy.setOrigin(20, 20);
  sGameover.setPosition(100, 100);

  for (int i = 0; i < M; ++i)
    for (int j = 0; j < N; ++j)
      grid[i][j] = (i == 0 || j == 0 || i == M - 1 || j == N - 1) ? 1 : 0;

  int enemyCount = 4;
  Enemy enemies[20];

  bool running = true;
  bool inGame = false;
  bool isLoadingSave = false;

  int playerX = 10, playerY = 0, dx = 0, dy = 0;
  int prevPlayerX = playerX, prevPlayerY = playerY;
  int reqDX = 0, reqDY = 0;
  bool movedThisTick = false;
  int lastCapturedX = -1, lastCapturedY = -1;
  float timer = 0.f, delay = 0.07f;
  Clock clock;

  Leaderboard leaderboard("leaderboard.txt");

  LevelManager levelManager("level.cfg");
  int currentLevelNumber = levelManager.loadLastSelected(); // 1-based
  LevelInfo currentLevel = levelManager.getLevel(currentLevelNumber);

  Auth auth("users.txt");
  const Player *currentPlayer = nullptr;

  ScoreSystem scoreSys;
  MultiplayerManager multiMgr;
  Matchmaking matchmaker(128, 256);

  FriendSystem friendsSys(256, 1024);

  Inventory inventory;

  // Load current theme
  string currentThemeId = "";
  Theme currentTheme;
  Color menuBgColor = Color(135, 206, 250); // Default Firewatch-style sky blue
  if (currentPlayer)
  {
    inventory.loadUserTheme(currentPlayer->username, currentThemeId);
    if (!currentThemeId.empty())
    {
      inventory.searchTheme(currentThemeId, currentTheme);
      menuBgColor = getThemeBackgroundColor(currentTheme, menuBgColor);
    }
  }

  if (!showAuthScreen(window, font, auth, currentPlayer))
  {
    return 0;
  }

  // Reload theme after login
  if (currentPlayer)
  {
    currentThemeId = "";
    inventory.loadUserTheme(currentPlayer->username, currentThemeId);
    if (!currentThemeId.empty())
    {
      inventory.searchTheme(currentThemeId, currentTheme);
      menuBgColor = getThemeBackgroundColor(currentTheme, menuBgColor);
    }

    int inv = 0;
    if (loadUserInventory(currentPlayer->username, inv))
    {
      scoreSys.setInventory(inv);
    }
  }

  if (currentPlayer)
  {
    friendsSys.ensurePlayer(currentPlayer->username);
  }

  friendsSys.ensurePlayersFromFile("users.txt");
  friendsSys.loadFromFile("friends.txt");

  while (window.isOpen() && running)
  {
    int selected = 0;
    const int OPTIONS = 11;
    string items[OPTIONS] = {"Start Single Player",
                             "Start Multiplayer",
                             "Matchmaking Queue",
                             "Select Level",
                             "Friends",
                             "Inventory",
                             "Profile",
                             "Leaderboard",
                             "Load Save",
                             "Logout",
                             "Exit"};

    bool inStartMenu = true;
    auto runMatchmaking = [&]()
    {
      bool inQueueScreen = true;
      GameRoom recentRooms[16];
      int recentCount = 0;

      while (inQueueScreen && window.isOpen())
      {
        Event qev;
        while (window.pollEvent(qev))
        {
          if (qev.type == Event::Closed)
          {
            window.close();
            running = false;
            inQueueScreen = false;
            break;
          }
          if (qev.type == Event::KeyPressed)
          {
            if (qev.key.code == Keyboard::Escape)
            {
              inQueueScreen = false;
              break;
            }
            if (qev.key.code == Keyboard::E)
            {
              string uname = currentPlayer
                                 ? currentPlayer->username
                                 : string("Guest_") + to_string((int)time(0));
              matchmaker.enqueuePlayer(uname, scoreSys.getScore());
            }
            if (qev.key.code == Keyboard::B)
            {
              string bname = string("Bot") + to_string(rand() % 10000);
              int bscore = rand() % 2000;
              matchmaker.enqueuePlayer(bname, bscore);
            }
            if (qev.key.code == Keyboard::M)
            {
              GameRoom gr;
              if (matchmaker.matchNextPair(gr))
              {
                if (recentCount == 16)
                {
                  for (int r = 1; r < 16; ++r)
                    recentRooms[r - 1] = recentRooms[r];
                  recentRooms[15] = gr;
                }
                else
                {
                  recentRooms[recentCount++] = gr;
                }
                leaderboard.addScore(gr.p1.username.empty() ? string("Guest")
                                                            : gr.p1.username,
                                     gr.p1.score);
                leaderboard.addScore(gr.p2.username.empty() ? string("Guest")
                                                            : gr.p2.username,
                                     gr.p2.score);
              }
            }
            if (qev.key.code == Keyboard::A)
            {
              GameRoom rooms[16];
              int n = matchmaker.matchAll(rooms, 16);
              for (int i = 0; i < n; ++i)
              {
                if (recentCount == 16)
                {
                  for (int r = 1; r < 16; ++r)
                    recentRooms[r - 1] = recentRooms[r];
                  recentRooms[15] = rooms[i];
                }
                else
                  recentRooms[recentCount++] = rooms[i];
                leaderboard.addScore(rooms[i].p1.username.empty()
                                         ? string("Guest")
                                         : rooms[i].p1.username,
                                     rooms[i].p1.score);
                leaderboard.addScore(rooms[i].p2.username.empty()
                                         ? string("Guest")
                                         : rooms[i].p2.username,
                                     rooms[i].p2.score);
              }
            }
          }
        }

        window.clear(Color(12, 12, 22));
        drawMenuText(window, font, "Matchmaking Queue", 28, 20.f, 20.f, true);
        string info = string("Pending Players: ") +
                      to_string(matchmaker.pendingPlayers());
        drawMenuText(window, font, info, 18, 20.f, 70.f, false);
        drawMenuText(window, font,
                     "Controls: E=Enqueue you, B=Add Bot, M=Match one, A=Match "
                     "all, ESC=Back",
                     14, 20.f, 100.f, false);

        drawMenuText(window, font, "Recent Matches:", 18, 20.f, 140.f, true);
        for (int i = 0; i < recentCount; ++i)
        {
          string line = to_string(recentRooms[i].roomId) + ": " +
                        recentRooms[i].p1.username + "(" +
                        to_string(recentRooms[i].p1.score) + ") vs " +
                        recentRooms[i].p2.username + "(" +
                        to_string(recentRooms[i].p2.score) + ")";
          drawMenuText(window, font, line, 16, 40.f, 180.f + i * 22.f, false);
        }

        window.display();
      }
    };
    auto runLocalMatch = [&]()
    {
      for (int i = 0; i < M; ++i)
        for (int j = 0; j < N; ++j)
          grid[i][j] = (i == 0 || j == 0 || i == M - 1 || j == N - 1) ? 1 : 0;

      multiMgr.resetPlayers();
      PlayerState &p1 = multiMgr.p1;
      PlayerState &p2 = multiMgr.p2;

      // place players inside arena
      p1.x = 5;
      p1.y = 5;
      p1.dx = p1.dy = 0;
      p1.alive = true;
      p2.x = N - 6;
      p2.y = M - 6;
      p2.dx = p2.dy = 0;
      p2.alive = true;

      int localEnemyCount = currentLevel.enemyCount;
      if (localEnemyCount < 1)
        localEnemyCount = 1;
      if (localEnemyCount > 20)
        localEnemyCount = 20;

      for (int i = 0; i < localEnemyCount; ++i)
      {
        int ex = (rand() % (N - 4)) + 2;
        int ey = (rand() % (M - 4)) + 2;
        enemies[i].x = ex * ts + ts / 2;
        enemies[i].y = ey * ts + ts / 2;
        enemies[i].dx = (rand() % 3) + 1;
        if (rand() % 2)
          enemies[i].dx = -enemies[i].dx;
        enemies[i].dy = (rand() % 3) + 1;
        if (rand() % 2)
          enemies[i].dy = -enemies[i].dy;
      }

      ScoreSystem score1, score2;
      score1.reset(true);
      score2.reset(true);

      Clock lclock;
      float ltimer = 0.f;
      float ldelay = currentLevel.delay;
      bool p1dead = false, p2dead = false;
      int p1LastCapturedX = -1, p1LastCapturedY = -1;
      int p2LastCapturedX = -1, p2LastCapturedY = -1;
      bool p1MovedThisTick = false, p2MovedThisTick = false;
      int p1PrevX = p1.x, p1PrevY = p1.y;
      int p2PrevX = p2.x, p2PrevY = p2.y;

      bool inLocal = true;
      while (inLocal && window.isOpen() && (!p1dead || !p2dead))
      {
        float dt = lclock.restart().asSeconds();
        ltimer += dt;
        Event ev;
        while (window.pollEvent(ev))
        {
          if (ev.type == Event::Closed)
          {
            window.close();
            running = false;
            inLocal = false;
            break;
          }
          if (ev.type == Event::KeyPressed)
          {
            if (ev.key.code == Keyboard::Escape)
            {
              inLocal = false;
              break;
            }
            if (ev.key.code == Keyboard::Up)
            {
              p1.dx = 0;
              p1.dy = -1;
            }
            if (ev.key.code == Keyboard::Down)
            {
              p1.dx = 0;
              p1.dy = 1;
            }
            if (ev.key.code == Keyboard::Left)
            {
              p1.dx = -1;
              p1.dy = 0;
            }
            if (ev.key.code == Keyboard::Right)
            {
              p1.dx = 1;
              p1.dy = 0;
            }
            if (ev.key.code == Keyboard::Q)
            {
              p2.dx = 0;
              p2.dy = -1;
            }
            if (ev.key.code == Keyboard::G)
            {
              p2.dx = 0;
              p2.dy = 1;
            }
            if (ev.key.code == Keyboard::A)
            {
              p2.dx = -1;
              p2.dy = 0;
            }
            if (ev.key.code == Keyboard::X)
            {
              p2.dx = 1;
              p2.dy = 0;
            }
          }
        }

        if (ltimer > ldelay)
        {
          p1MovedThisTick = false;
          p2MovedThisTick = false;
          p1PrevX = p1.x;
          p1PrevY = p1.y;
          p2PrevX = p2.x;
          p2PrevY = p2.y;

          if (!p1dead)
          {
            if (p1.dx != 0 || p1.dy != 0)
            {
              p1.x += p1.dx;
              p1.y += p1.dy;
              if (p1.x < 0)
                p1.x = 0;
              if (p1.x > N - 1)
                p1.x = N - 1;
              if (p1.y < 0)
                p1.y = 0;
              if (p1.y > M - 1)
                p1.y = M - 1;

              p1MovedThisTick = (p1.x != p1PrevX || p1.y != p1PrevY);

              if (grid[p1.y][p1.x] == 2)
              {
                p1dead = true;
              }
              else if (grid[p1.y][p1.x] == 1 && p1MovedThisTick &&
                       (p1.x != p1LastCapturedX || p1.y != p1LastCapturedY))
              {
                p1.dx = p1.dy = 0;
                for (int i = 0; i < localEnemyCount; ++i)
                {
                  drop(enemies[i].y / ts, enemies[i].x / ts);
                }
                int captured = 0;
                for (int i = 0; i < M; ++i)
                {
                  for (int j = 0; j < N; ++j)
                  {
                    if (grid[i][j] == -1)
                      grid[i][j] = 0;
                    else if (grid[i][j] == 0)
                    {
                      grid[i][j] = 1;
                      captured++;
                    }
                    else if (grid[i][j] == 2)
                    {
                      grid[i][j] = 1;
                      captured++;
                    }
                  }
                }
                if (captured > 0)
                {
                  score1.registerCapture(captured);
                }
                p1LastCapturedX = p1.x;
                p1LastCapturedY = p1.y;
              }
              else if (grid[p1.y][p1.x] == 0)
              {
                grid[p1.y][p1.x] = 2;
                if (p1MovedThisTick)
                {
                  p1LastCapturedX = -1;
                  p1LastCapturedY = -1;
                }
              }
            }
            else
            {
              p1MovedThisTick = false;
            }
          }
          if (!p2dead)
          {
            if (p2.dx != 0 || p2.dy != 0)
            {
              p2.x += p2.dx;
              p2.y += p2.dy;
              if (p2.x < 0)
                p2.x = 0;
              if (p2.x > N - 1)
                p2.x = N - 1;
              if (p2.y < 0)
                p2.y = 0;
              if (p2.y > M - 1)
                p2.y = M - 1;

              p2MovedThisTick = (p2.x != p2PrevX || p2.y != p2PrevY);

              if (grid[p2.y][p2.x] == 3)
              {
                p2dead = true;
              }
              else if (grid[p2.y][p2.x] == 1 && p2MovedThisTick &&
                       (p2.x != p2LastCapturedX || p2.y != p2LastCapturedY))
              {
                p2.dx = p2.dy = 0;
                for (int i = 0; i < localEnemyCount; ++i)
                {
                  drop(enemies[i].y / ts, enemies[i].x / ts);
                }
                int captured = 0;
                for (int i = 0; i < M; ++i)
                {
                  for (int j = 0; j < N; ++j)
                  {
                    if (grid[i][j] == -1)
                      grid[i][j] = 0;
                    else if (grid[i][j] == 0)
                    {
                      grid[i][j] = 1;
                      captured++;
                    }
                    else if (grid[i][j] == 3)
                    {
                      grid[i][j] = 1;
                      captured++;
                    }
                  }
                }
                if (captured > 0)
                {
                  score2.registerCapture(captured);
                }
                p2LastCapturedX = p2.x;
                p2LastCapturedY = p2.y;
              }
              else if (grid[p2.y][p2.x] == 0)
              {
                grid[p2.y][p2.x] = 3;
                if (p2MovedThisTick)
                {
                  p2LastCapturedX = -1;
                  p2LastCapturedY = -1;
                }
              }
            }
            else
            {
              p2MovedThisTick = false;
            }
          }

          if (!p1dead)
            score1.update(ltimer);
          if (!p2dead)
            score2.update(ltimer);

          ltimer = 0.f;
        }

        if (!p1dead || !p2dead)
        {
          for (int i = 0; i < localEnemyCount; ++i)
            enemies[i].move();

          for (int i = 0; i < localEnemyCount; ++i)
          {
            int gx = enemies[i].x / ts;
            int gy = enemies[i].y / ts;
            if (gx >= 0 && gx < N && gy >= 0 && gy < M)
            {
              if (grid[gy][gx] == 2)
                p1dead = true;
              if (grid[gy][gx] == 3)
                p2dead = true;
            }
          }
        }

        window.clear(Color(6, 8, 18));

        for (int i = 0; i < M; ++i)
        {
          for (int j = 0; j < N; ++j)
          {
            if (grid[i][j] == 0)
              continue;
            if (grid[i][j] == 1)
            {
              sTile.setTextureRect(IntRect(0, 0, ts, ts));
              sTile.setColor(Color::White);
            }
            else if (grid[i][j] == 2)
            {
              sTile.setTextureRect(IntRect(54, 0, ts, ts));
              sTile.setColor(Color(220, 120, 120));
            }
            else if (grid[i][j] == 3)
            {
              sTile.setTextureRect(IntRect(54, 0, ts, ts));
              sTile.setColor(Color(120, 200, 140));
            }
            sTile.setPosition(j * ts, i * ts);
            window.draw(sTile);
          }
        }
        sTile.setColor(Color::White);

        for (int i = 0; i < localEnemyCount; ++i)
        {
          sEnemy.setPosition(enemies[i].x, enemies[i].y);
          window.draw(sEnemy);
        }

        // draw players (on top)
        CircleShape c1(ts / 2 - 2);
        c1.setFillColor(Color(220, 80, 80));
        c1.setPosition(p1.x * ts + 2, p1.y * ts + 2);
        window.draw(c1);
        CircleShape c2(ts / 2 - 2);
        c2.setFillColor(Color(80, 220, 120));
        c2.setPosition(p2.x * ts + 2, p2.y * ts + 2);
        window.draw(c2);

        float winW = (float)window.getSize().x;
        const float hudH = 40.f;
        RectangleShape hudBar(Vector2f(winW, hudH));
        hudBar.setPosition(0.f, 0.f);
        hudBar.setFillColor(Color(8, 8, 16, 200));
        window.draw(hudBar);

        string p1nameDisplay = currentPlayer ? (currentPlayer->nickname.empty()
                                                    ? currentPlayer->username
                                                    : currentPlayer->nickname)
                                             : string("P1");
        drawMenuText(window, font,
                     string("P1: ") + to_string(score1.getScore()), 16, 12.f,
                     8.f, true);

        string enemiesStr = string("Enemies: ") + to_string(localEnemyCount);
        drawMenuText(window, font, enemiesStr, 14,
                     winW / 2.f - (enemiesStr.size() * 4.f), 10.f, false);

        string p2s = string("P2: ") + to_string(score2.getScore());
        drawMenuText(window, font, p2s, 16, winW - 120.f, 8.f, true);

        if (p1dead || p2dead)
        {
          float ow = 380.f, oh = 110.f;
          RectangleShape overlay(Vector2f(ow, oh));
          overlay.setPosition((winW - ow) / 2.f,
                              (float)window.getSize().y / 2.f - oh / 2.f);
          overlay.setFillColor(Color(12, 14, 22, 220));
          overlay.setOutlineColor(Color(180, 80, 80));
          overlay.setOutlineThickness(2.f);
          window.draw(overlay);

          string status;
          if (p1dead && !p2dead)
            status = string(" PLAYER 2 WIN");
          else if (p2dead && !p1dead)
            status = string(" PLAYER 1 WIN");
          else
            status = string("BOTH ELIMINATED");
          drawMenuText(window, font, status, 22, overlay.getPosition().x + 18.f,
                       overlay.getPosition().y + 12.f, true);
          string scoresLine = string("P1: ") + to_string(score1.getScore()) +
                              string("   P2: ") + to_string(score2.getScore());
          drawMenuText(window, font, scoresLine, 18,
                       overlay.getPosition().x + 18.f,
                       overlay.getPosition().y + 52.f, false);
          drawMenuText(window, font, string("Press ESC to return"), 14,
                       overlay.getPosition().x + 18.f,
                       overlay.getPosition().y + 80.f, false);
        }

        window.display();
      }

      int sc1 = p1dead ? 0 : 100;
      int sc2 = p2dead ? 0 : 100;
      if (!window.isOpen())
        return;
      string winner;
      if (p1dead && !p2dead)
        winner = (currentPlayer ? currentPlayer->username : string("Player1"));
      else if (p2dead && !p1dead)
        winner = string("Player2");
      else
        winner = string("Draw");

      if (winner != "Draw")
        leaderboard.addScore(winner, max(sc1, sc2));
      bool showing = true;
      int sel = 0;
      string opts[2] = {"Return", "Exit"};
      while (showing && window.isOpen())
      {
        Event ev;
        while (window.pollEvent(ev))
        {
          if (ev.type == Event::Closed)
          {
            window.close();
            showing = false;
            break;
          }
          if (ev.type == Event::KeyPressed)
          {
            if (ev.key.code == Keyboard::Return)
            {
              if (sel == 0)
                showing = false;
              else
              {
                window.close();
                showing = false;
                running = false;
              }
            }
            if (ev.key.code == Keyboard::Up)
              sel = (sel - 1 + 2) % 2;
            if (ev.key.code == Keyboard::Down)
              sel = (sel + 1) % 2;
          }
        }
        window.clear(Color(20, 20, 30));
        drawMenuText(window, font, string("Match Ended - ") + winner, 26, 20.f,
                     20.f, true);
        for (int i = 0; i < 2; ++i)
        {
          bool s = (i == sel);
          drawMenuText(window, font, (s ? "> " : "  ") + opts[i], (s ? 22 : 18),
                       40.f, 80.f + i * 32.f, s);
        }
        window.display();
      }
    };
    while (inStartMenu && window.isOpen())
    {
      Event ev;
      while (window.pollEvent(ev))
      {
        if (ev.type == Event::Closed)
        {
          window.close();
          running = false;
          inStartMenu = false;
          break;
        }
        if (ev.type == Event::KeyPressed)
        {
          if (ev.key.code == Keyboard::Up)
            selected = (selected - 1 + OPTIONS) % OPTIONS;
          if (ev.key.code == Keyboard::Down)
            selected = (selected + 1) % OPTIONS;
          if (ev.key.code == Keyboard::Return)
          {
            // handle selection
            if (selected == 0)
            { // Start Single
              inStartMenu = false;
              inGame = true;
              isLoadingSave = false; // new game, not loading
            }
            else if (selected == 1)
            {
              // Start Multiplayer selected: open local multiplayer screen
              runLocalMatch();
              inStartMenu = true;
            }
            else if (selected == 2)
            {
              runMatchmaking();
              inStartMenu = true;
            }
            else if (selected == 3)
            {
              int lvlSel = currentLevelNumber - 1;
              bool inLevelMenu = true;
              while (inLevelMenu && window.isOpen())
              {
                Event ev3;
                while (window.pollEvent(ev3))
                {
                  if (ev3.type == Event::Closed)
                  {
                    window.close();
                    inLevelMenu = false;
                    inStartMenu = false;
                    running = false;
                    break;
                  }
                  if (ev3.type == Event::KeyPressed)
                  {
                    if (ev3.key.code == Keyboard::Up)
                      lvlSel = (lvlSel - 1 + levelManager.getCount()) %
                               levelManager.getCount();
                    if (ev3.key.code == Keyboard::Down)
                      lvlSel = (lvlSel + 1) % levelManager.getCount();
                    if (ev3.key.code == Keyboard::Return)
                    {
                      currentLevelNumber = lvlSel + 1;
                      currentLevel = levelManager.getLevel(currentLevelNumber);
                      levelManager.saveLastSelected(currentLevelNumber);
                      inLevelMenu = false;
                      break;
                    }
                    if (ev3.key.code == Keyboard::Escape)
                    {
                      inLevelMenu = false;
                      break;
                    }
                  }
                }

                // Modern dark theme for level selection
                window.clear(Color(25, 30, 40));
                RectangleShape titleBar(Vector2f(window.getSize().x, 70.f));
                titleBar.setPosition(0, 0);
                titleBar.setFillColor(Color(40, 50, 70));
                window.draw(titleBar);
                drawMenuText(window, font, "Select Level", 32,
                             window.getSize().x / 2.f - 100.f, 20.f, true,
                             Color::White);

                float startY = 100.f;
                for (int i = 0; i < levelManager.getCount(); ++i)
                {
                  LevelInfo li = levelManager.getLevel(i + 1);
                  bool sel = (i == lvlSel);
                  Color itemColor = sel ? Color(255, 200, 50) : Color::White;
                  string line = li.name;
                  drawMenuText(window, font, line, (sel ? 20 : 18), 40.f,
                               startY + i * 32.f, sel, itemColor);
                }
                drawMenuText(window, font,
                             "Use Up/Down + Enter to select, ESC to cancel", 14,
                             20.f,
                             startY + levelManager.getCount() * 32.f + 20.f,
                             false, Color(180, 180, 180));
                window.display();
              }
            }
            else if (selected == 4)
            {
              bool inFriends = true;
              int fsel = 0;
              const int FOPTS = 5;
              string fopts[FOPTS] = {"View Friends", "View Requests",
                                     "Send Request", "Search User", "Back"};

              while (inFriends && window.isOpen())
              {
                Event fev;
                while (window.pollEvent(fev))
                {
                  if (fev.type == Event::Closed)
                  {
                    window.close();
                    running = false;
                    inFriends = false;
                    break;
                  }
                  if (fev.type == Event::KeyPressed)
                  {
                    if (fev.key.code == Keyboard::Up)
                      fsel = (fsel - 1 + FOPTS) % FOPTS;
                    if (fev.key.code == Keyboard::Down)
                      fsel = (fsel + 1) % FOPTS;
                    if (fev.key.code == Keyboard::Return)
                    {
                      if (fsel == 0)
                      {
                        // View friends list
                        int cnt = currentPlayer ? friendsSys.getFriendCount(
                                                      currentPlayer->username)
                                                : 0;
                        bool viewing = true;
                        int idx = 0;
                        while (viewing && window.isOpen())
                        {
                          Event evf;
                          while (window.pollEvent(evf))
                          {
                            if (evf.type == Event::Closed)
                            {
                              window.close();
                              viewing = false;
                              break;
                            }
                            if (evf.type == Event::KeyPressed)
                            {
                              if (evf.key.code == Keyboard::Escape)
                              {
                                viewing = false;
                                break;
                              }
                              if (cnt > 0 && evf.key.code == Keyboard::Up)
                                idx = (idx - 1 + cnt) % cnt;
                              if (cnt > 0 && evf.key.code == Keyboard::Down)
                                idx = (idx + 1) % cnt;
                            }
                          }
                          window.clear(Color(25, 30, 40));
                          RectangleShape titleBar(
                              Vector2f(window.getSize().x, 70.f));
                          titleBar.setPosition(0, 0);
                          titleBar.setFillColor(Color(40, 50, 70));
                          window.draw(titleBar);
                          drawMenuText(window, font, "Friends List", 28,
                                       window.getSize().x / 2.f - 80.f, 20.f,
                                       true, Color::White);

                          float startY = 100.f;
                          for (int i = 0; i < cnt; ++i)
                          {
                            string nm;
                            friendsSys.getFriendAt(currentPlayer->username, i,
                                                   nm);
                            bool sel = (i == idx);
                            Color itemColor =
                                sel ? Color(255, 200, 50) : Color::White;
                            drawMenuText(window, font, nm, (sel ? 20 : 18),
                                         40.f, startY + i * 30.f, sel,
                                         itemColor);
                          }
                          drawMenuText(window, font, "ESC to return", 14, 20.f,
                                       startY + (cnt ? cnt : 1) * 30.f + 20.f,
                                       false, Color(180, 180, 180));
                          window.display();
                        }
                      }
                      else if (fsel == 1)
                      {
                        int pcount = currentPlayer
                                         ? friendsSys.getPendingCount(
                                               currentPlayer->username)
                                         : 0;
                        bool viewing = true;
                        int pidx = 0;
                        while (viewing && window.isOpen())
                        {
                          Event evf;
                          while (window.pollEvent(evf))
                          {
                            if (evf.type == Event::Closed)
                            {
                              window.close();
                              viewing = false;
                              break;
                            }
                            if (evf.type == Event::KeyPressed)
                            {
                              if (evf.key.code == Keyboard::Escape)
                              {
                                viewing = false;
                                break;
                              }
                              if (pcount > 0 && evf.key.code == Keyboard::Up)
                                pidx = (pidx - 1 + pcount) % pcount;
                              if (pcount > 0 && evf.key.code == Keyboard::Down)
                                pidx = (pidx + 1) % pcount;
                              if (evf.key.code == Keyboard::A)
                              {
                                string from;
                                if (friendsSys.getPendingAt(
                                        currentPlayer->username, pidx, from))
                                {
                                  friendsSys.acceptFriendRequest(
                                      currentPlayer->username, from);
                                  pcount = friendsSys.getPendingCount(
                                      currentPlayer->username);
                                  if (pidx >= pcount)
                                    pidx = pcount - 1;
                                }
                              }
                              if (evf.key.code == Keyboard::R)
                              {
                                string from;
                                if (friendsSys.getPendingAt(
                                        currentPlayer->username, pidx, from))
                                {
                                  friendsSys.rejectFriendRequest(
                                      currentPlayer->username, from);
                                  pcount = friendsSys.getPendingCount(
                                      currentPlayer->username);
                                  if (pidx >= pcount)
                                    pidx = pcount - 1;
                                }
                              }
                            }
                          }
                          window.clear(Color(25, 30, 40));
                          RectangleShape titleBar(
                              Vector2f(window.getSize().x, 70.f));
                          titleBar.setPosition(0, 0);
                          titleBar.setFillColor(Color(40, 50, 70));
                          window.draw(titleBar);
                          drawMenuText(window, font, "Pending Requests", 28,
                                       window.getSize().x / 2.f - 120.f, 20.f,
                                       true, Color::White);

                          float startY = 100.f;
                          for (int i = 0; i < pcount; ++i)
                          {
                            string nm;
                            friendsSys.getPendingAt(currentPlayer->username, i,
                                                    nm);
                            bool sel = (i == pidx);
                            Color itemColor =
                                sel ? Color(255, 200, 50) : Color::White;
                            drawMenuText(window, font, nm, (sel ? 20 : 18),
                                         40.f, startY + i * 30.f, sel,
                                         itemColor);
                          }
                          drawMenuText(window, font,
                                       "A=Accept  R=Reject  ESC=Back", 14, 20.f,
                                       startY + (pcount ? pcount : 1) * 30.f +
                                           20.f,
                                       false, Color(180, 180, 180));
                          window.display();
                        }
                      }
                      else if (fsel == 2)
                      {
                        string target = showTextInput(
                            window, font, "Send request to (username):", "",
                            32);
                        if (!target.empty() && currentPlayer)
                        {
                          friendsSys.ensurePlayer(target);
                          friendsSys.ensurePlayer(currentPlayer->username);
                          bool ok = friendsSys.sendFriendRequest(
                              currentPlayer->username, target);
                          float ttime = 0.f;
                          Clock rtc;
                          while (ttime < 1.2f && window.isOpen())
                          {
                            Event fve;
                            while (window.pollEvent(fve))
                            {
                              if (fve.type == Event::Closed)
                              {
                                window.close();
                                running = false;
                                break;
                              }
                            }
                            window.clear(Color(16, 18, 26));
                            drawMenuText(window, font,
                                         ok ? string("Request sent to ") +
                                                  target
                                            : string("Request failed or "
                                                     "already sent/friends"),
                                         18, 20.f, 20.f, false);
                            window.display();
                            ttime = rtc.getElapsedTime().asSeconds();
                          }
                        }
                      }
                      else if (fsel == 3)
                      {
                        string target = showTextInput(
                            window, font, "Search username:", "", 32);
                        if (!target.empty())
                        {
                          int tidx = friendsSys.findIndex(target);
                          if (tidx < 0)
                          {
                            float ttime = 0.f;
                            Clock rtc;
                            while (ttime < 1.2f && window.isOpen())
                            {
                              Event fe;
                              while (window.pollEvent(fe))
                              {
                                if (fe.type == Event::Closed)
                                {
                                  window.close();
                                  running = false;
                                  break;
                                }
                              }
                              window.clear(Color(18, 18, 28));
                              drawMenuText(window, font,
                                           string("User not found: ") + target,
                                           18, 20.f, 20.f, false);
                              window.display();
                              ttime = rtc.getElapsedTime().asSeconds();
                            }
                          }
                          else
                          {
                            bool viewing = true;
                            while (viewing && window.isOpen())
                            {
                              Event pev;
                              while (window.pollEvent(pev))
                              {
                                if (pev.type == Event::Closed)
                                {
                                  window.close();
                                  viewing = false;
                                  break;
                                }
                                if (pev.type == Event::KeyPressed)
                                {
                                  if (pev.key.code == Keyboard::Escape)
                                  {
                                    viewing = false;
                                    break;
                                  }
                                  if (pev.key.code == Keyboard::S)
                                  {
                                    if (currentPlayer)
                                      friendsSys.sendFriendRequest(
                                          currentPlayer->username, target);
                                  }
                                }
                              }
                              window.clear(Color(16, 16, 26));
                              drawMenuText(window, font,
                                           string("Profile: ") + target, 22,
                                           20.f, 16.f, true);
                              int myCnt = currentPlayer
                                              ? friendsSys.getFriendCount(
                                                    currentPlayer->username)
                                              : 0;
                              int theirCnt = friendsSys.getFriendCount(target);
                              int mutual = 0;
                              for (int i = 0; i < myCnt; ++i)
                              {
                                string a;
                                friendsSys.getFriendAt(currentPlayer->username,
                                                       i, a);
                                for (int j = 0; j < theirCnt; ++j)
                                {
                                  string b;
                                  friendsSys.getFriendAt(target, j, b);
                                  if (a == b)
                                    ++mutual;
                                }
                              }
                              drawMenuText(
                                  window, font,
                                  string("Friends: ") + to_string(theirCnt) +
                                      string("  Mutual: ") + to_string(mutual),
                                  16, 20.f, 60.f, false);
                              int show = theirCnt < 10 ? theirCnt : 10;
                              for (int i = 0; i < show; ++i)
                              {
                                string nm;
                                friendsSys.getFriendAt(target, i, nm);
                                drawMenuText(window, font, nm, 16, 24.f,
                                             100.f + i * 22.f, false);
                              }
                              drawMenuText(window, font,
                                           "S=Send Request  ESC=Back", 14, 20.f,
                                           100.f + show * 22.f + 12.f, false);
                              window.display();
                            }
                          }
                        }
                      }
                      else
                      {
                        inFriends = false;
                        break;
                      }
                    }
                  }
                }

                window.clear(Color(25, 30, 40));
                RectangleShape titleBar(Vector2f(window.getSize().x, 70.f));
                titleBar.setPosition(0, 0);
                titleBar.setFillColor(Color(40, 50, 70));
                window.draw(titleBar);
                drawMenuText(window, font, "Friends - Menu", 32,
                             window.getSize().x / 2.f - 100.f, 20.f, true,
                             Color::White);

                float startY = 100.f;
                for (int i = 0; i < FOPTS; ++i)
                {
                  bool s = (i == fsel);
                  Color itemColor = s ? Color(255, 200, 50) : Color::White;
                  drawMenuText(window, font, fopts[i], (s ? 22 : 18), 40.f,
                               startY + i * 35.f, s, itemColor);
                }
                drawMenuText(
                    window, font,
                    "Use Up/Down + Enter. In Requests: A=Accept R=Reject.", 14,
                    20.f, startY + FOPTS * 35.f + 20.f, false,
                    Color(180, 180, 180));
                window.display();
              }
            }
            else if (selected == 5)
            {
              bool inInventory = true;
              int invSel = 0;
              const int INVOPTS = 4;
              string invOpts[INVOPTS] = {"Browse Themes", "Search Theme by ID",
                                         "View Current Theme", "Back"};

              string currentThemeId = "";
              if (currentPlayer)
              {
                inventory.loadUserTheme(currentPlayer->username,
                                        currentThemeId);
              }

              while (inInventory && window.isOpen())
              {
                Event iev;
                while (window.pollEvent(iev))
                {
                  if (iev.type == Event::Closed)
                  {
                    window.close();
                    running = false;
                    inInventory = false;
                    break;
                  }
                  if (iev.type == Event::KeyPressed)
                  {
                    if (iev.key.code == Keyboard::Up)
                      invSel = (invSel - 1 + INVOPTS) % INVOPTS;
                    if (iev.key.code == Keyboard::Down)
                      invSel = (invSel + 1) % INVOPTS;
                    if (iev.key.code == Keyboard::Return)
                    {
                      if (invSel == 0)
                      { // Browse Themes
                        Theme themes[100];
                        int themeCount = inventory.getAllThemes(themes, 100);
                        bool browsing = true;
                        int themeIdx = 0;

                        while (browsing && window.isOpen())
                        {
                          Event tev;
                          while (window.pollEvent(tev))
                          {
                            if (tev.type == Event::Closed)
                            {
                              window.close();
                              browsing = false;
                              inInventory = false;
                              running = false;
                              break;
                            }
                            if (tev.type == Event::KeyPressed)
                            {
                              if (tev.key.code == Keyboard::Escape)
                              {
                                browsing = false;
                                break;
                              }
                              if (themeCount > 0 &&
                                  tev.key.code == Keyboard::Up)
                                themeIdx =
                                    (themeIdx - 1 + themeCount) % themeCount;
                              if (themeCount > 0 &&
                                  tev.key.code == Keyboard::Down)
                                themeIdx = (themeIdx + 1) % themeCount;
                              if (tev.key.code == Keyboard::Return &&
                                  themeCount > 0)
                              {
                                // Select this theme
                                if (currentPlayer)
                                {
                                  inventory.saveUserTheme(
                                      currentPlayer->username,
                                      themes[themeIdx].themeId);
                                  currentThemeId = themes[themeIdx].themeId;
                                  currentTheme = themes[themeIdx];
                                  menuBgColor = getThemeBackgroundColor(
                                      currentTheme, Color(135, 206, 250));
                                }
                                browsing = false;
                              }
                            }
                          }

                          // Modern dark theme for browse themes
                          window.clear(Color(25, 30, 40));
                          RectangleShape titleBar(
                              Vector2f(window.getSize().x, 70.f));
                          titleBar.setPosition(0, 0);
                          titleBar.setFillColor(Color(40, 50, 70));
                          window.draw(titleBar);
                          drawMenuText(window, font, "Browse Themes (In-Order)",
                                       28, window.getSize().x / 2.f - 150.f,
                                       20.f, true, Color::White);

                          int startIdx = (themeIdx > 8) ? themeIdx - 8 : 0;
                          int endIdx = (startIdx + 18 < themeCount)
                                           ? startIdx + 18
                                           : themeCount;

                          for (int i = startIdx; i < endIdx; ++i)
                          {
                            bool sel = (i == themeIdx);
                            Color itemColor =
                                sel ? Color(255, 200, 50) : Color::White;
                            string line =
                                themes[i].themeId + ": " + themes[i].themeName;
                            drawMenuText(window, font, line, (sel ? 20 : 18),
                                         40.f, 100.f + (i - startIdx) * 28.f,
                                         sel, itemColor);
                          }

                          drawMenuText(window, font,
                                       "Up/Down to navigate, Enter to select, "
                                       "ESC to back",
                                       14, 20.f,
                                       100.f + (endIdx - startIdx) * 28.f +
                                           30.f,
                                       false, Color(180, 180, 180));
                          window.display();
                        }
                      }
                      else if (invSel == 1)
                      { // Search Theme by ID
                        string searchId = showTextInput(
                            window, font, "Enter Theme ID to search:", "", 16);
                        if (!searchId.empty())
                        {
                          Theme found;
                          if (inventory.searchTheme(searchId, found))
                          {
                            bool viewing = true;
                            while (viewing && window.isOpen())
                            {
                              Event sev;
                              while (window.pollEvent(sev))
                              {
                                if (sev.type == Event::Closed)
                                {
                                  window.close();
                                  viewing = false;
                                  inInventory = false;
                                  running = false;
                                  break;
                                }
                                if (sev.type == Event::KeyPressed)
                                {
                                  if (sev.key.code == Keyboard::Escape)
                                  {
                                    viewing = false;
                                    break;
                                  }
                                  if (sev.key.code == Keyboard::Return &&
                                      currentPlayer)
                                  {
                                    // Select this theme
                                    inventory.saveUserTheme(
                                        currentPlayer->username, found.themeId);
                                    currentThemeId = found.themeId;
                                    currentTheme = found;
                                    menuBgColor = getThemeBackgroundColor(
                                        currentTheme, Color(135, 206, 250));
                                    viewing = false;
                                  }
                                }
                              }
                              window.clear(Color(25, 30, 40));
                              RectangleShape titleBar(
                                  Vector2f(window.getSize().x, 70.f));
                              titleBar.setPosition(0, 0);
                              titleBar.setFillColor(Color(40, 50, 70));
                              window.draw(titleBar);
                              drawMenuText(window, font, "Theme Found", 28,
                                           window.getSize().x / 2.f - 80.f,
                                           20.f, true, Color::White);

                              float yPos = 100.f;
                              drawMenuText(window, font, "ID: " + found.themeId,
                                           20, 40.f, yPos, false,
                                           Color(220, 220, 220));
                              yPos += 35.f;
                              drawMenuText(window, font,
                                           "Name: " + found.themeName, 20, 40.f,
                                           yPos, false, Color(220, 220, 220));
                              yPos += 35.f;
                              drawMenuText(window, font,
                                           "Description: " + found.description,
                                           18, 40.f, yPos, false,
                                           Color(200, 200, 200));
                              yPos += 35.f;
                              drawMenuText(
                                  window, font, "Color: " + found.colorCode, 18,
                                  40.f, yPos, false, Color(200, 200, 200));
                              yPos += 40.f;
                              if (currentPlayer)
                              {
                                drawMenuText(window, font,
                                             "Enter to select, ESC to back", 14,
                                             40.f, yPos, false,
                                             Color(180, 180, 180));
                              }
                              else
                              {
                                drawMenuText(
                                    window, font,
                                    "ESC to back (Login to select theme)", 14,
                                    40.f, yPos, false, Color(180, 180, 180));
                              }
                              window.display();
                            }
                          }
                          else
                          {
                            float ttime = 0.f;
                            Clock rtc;
                            while (ttime < 1.5f && window.isOpen())
                            {
                              Event fe;
                              while (window.pollEvent(fe))
                              {
                                if (fe.type == Event::Closed)
                                {
                                  window.close();
                                  running = false;
                                  break;
                                }
                              }
                              window.clear(Color(18, 18, 28));
                              drawMenuText(window, font,
                                           string("Theme not found: ") +
                                               searchId,
                                           18, 20.f, 20.f, false);
                              window.display();
                              ttime = rtc.getElapsedTime().asSeconds();
                            }
                          }
                        }
                      }
                      else if (invSel == 2)
                      { // View Current Theme
                        if (currentThemeId.empty())
                        {
                          float ttime = 0.f;
                          Clock rtc;
                          while (ttime < 1.5f && window.isOpen())
                          {
                            Event fe;
                            while (window.pollEvent(fe))
                            {
                              if (fe.type == Event::Closed)
                              {
                                window.close();
                                running = false;
                                break;
                              }
                            }
                            window.clear(Color(18, 18, 28));
                            drawMenuText(window, font,
                                         currentPlayer
                                             ? "No theme selected"
                                             : "Login to select a theme",
                                         18, 20.f, 20.f, false);
                            window.display();
                            ttime = rtc.getElapsedTime().asSeconds();
                          }
                        }
                        else
                        {
                          Theme current;
                          if (inventory.searchTheme(currentThemeId, current))
                          {
                            bool viewing = true;
                            while (viewing && window.isOpen())
                            {
                              Event cev;
                              while (window.pollEvent(cev))
                              {
                                if (cev.type == Event::Closed)
                                {
                                  window.close();
                                  viewing = false;
                                  inInventory = false;
                                  running = false;
                                  break;
                                }
                                if (cev.type == Event::KeyPressed)
                                {
                                  if (cev.key.code == Keyboard::Escape)
                                  {
                                    viewing = false;
                                    break;
                                  }
                                }
                              }
                              window.clear(Color(25, 30, 40));
                              RectangleShape titleBar(
                                  Vector2f(window.getSize().x, 70.f));
                              titleBar.setPosition(0, 0);
                              titleBar.setFillColor(Color(40, 50, 70));
                              window.draw(titleBar);
                              drawMenuText(window, font, "Current Theme", 28,
                                           window.getSize().x / 2.f - 100.f,
                                           20.f, true, Color::White);

                              float yPos = 100.f;
                              drawMenuText(window, font,
                                           "ID: " + current.themeId, 20, 40.f,
                                           yPos, false, Color(220, 220, 220));
                              yPos += 35.f;
                              drawMenuText(
                                  window, font, "Name: " + current.themeName,
                                  20, 40.f, yPos, false, Color(220, 220, 220));
                              yPos += 35.f;
                              drawMenuText(
                                  window, font,
                                  "Description: " + current.description, 18,
                                  40.f, yPos, false, Color(200, 200, 200));
                              yPos += 35.f;
                              drawMenuText(
                                  window, font, "Color: " + current.colorCode,
                                  18, 40.f, yPos, false, Color(200, 200, 200));
                              yPos += 40.f;
                              drawMenuText(window, font, "ESC to back", 14,
                                           40.f, yPos, false,
                                           Color(180, 180, 180));
                              window.display();
                            }
                          }
                        }
                      }
                      else
                      { // Back
                        inInventory = false;
                        break;
                      }
                    }
                    if (iev.key.code == Keyboard::Escape)
                    {
                      inInventory = false;
                      break;
                    }
                  }
                }

                // Modern dark theme for inventory
                window.clear(Color(25, 30, 40));
                RectangleShape titleBar(Vector2f(window.getSize().x, 70.f));
                titleBar.setPosition(0, 0);
                titleBar.setFillColor(Color(40, 50, 70));
                window.draw(titleBar);
                drawMenuText(window, font, "Theme Inventory", 32,
                             window.getSize().x / 2.f - 120.f, 20.f, true,
                             Color::White);

                float startY = 100.f;
                for (int i = 0; i < INVOPTS; ++i)
                {
                  bool s = (i == invSel);
                  Color itemColor = s ? Color(255, 200, 50) : Color::White;
                  drawMenuText(window, font, invOpts[i], (s ? 22 : 18), 40.f,
                               startY + i * 35.f, s, itemColor);
                }
                if (!currentThemeId.empty())
                {
                  drawMenuText(window, font, "Current Theme: " + currentThemeId,
                               16, 40.f, startY + INVOPTS * 35.f + 20.f, false,
                               Color(200, 200, 200));
                }
                drawMenuText(window, font,
                             "Use Up/Down + Enter. ESC to return.", 14, 20.f,
                             startY + INVOPTS * 35.f + 50.f, false,
                             Color(180, 180, 180));
                window.display();
              }
            }
            else if (selected == 6)
            { // Profile
              if (!currentPlayer)
              {
                float ttime = 0.f;
                Clock rtc;
                while (ttime < 1.5f && window.isOpen())
                {
                  Event fe;
                  while (window.pollEvent(fe))
                  {
                    if (fe.type == Event::Closed)
                    {
                      window.close();
                      running = false;
                      break;
                    }
                  }
                  window.clear(Color(18, 18, 28));
                  drawMenuText(window, font, "Please login to view profile", 18,
                               20.f, 20.f, false);
                  window.display();
                  ttime = rtc.getElapsedTime().asSeconds();
                }
              }
              else
              {
                bool viewingProfile = true;
                int profileSel = 0;

                // Get player stats
                string username = currentPlayer->username;
                string nickname = currentPlayer->nickname.empty()
                                      ? username
                                      : currentPlayer->nickname;
                int friendCount = friendsSys.getFriendCount(username);

                // Get total points from leaderboard
                int totalPoints = 0;
                for (int i = 0; i < leaderboard.count(); ++i)
                {
                  string name;
                  int score;
                  if (leaderboard.getEntry(i, name, score))
                  {
                    if (name == username || name == nickname)
                    {
                      totalPoints = score;
                      break;
                    }
                  }
                }

                while (viewingProfile && window.isOpen())
                {
                  Event pev;
                  while (window.pollEvent(pev))
                  {
                    if (pev.type == Event::Closed)
                    {
                      window.close();
                      running = false;
                      viewingProfile = false;
                      break;
                    }
                    if (pev.type == Event::KeyPressed)
                    {
                      if (pev.key.code == Keyboard::Escape ||
                          pev.key.code == Keyboard::Return)
                      {
                        viewingProfile = false;
                        break;
                      }
                    }
                  }

                  // Modern dark theme for profile
                  window.clear(Color(25, 30, 40));
                  RectangleShape titleBar(Vector2f(window.getSize().x, 70.f));
                  titleBar.setPosition(0, 0);
                  titleBar.setFillColor(Color(40, 50, 70));
                  window.draw(titleBar);
                  drawMenuText(window, font, "Player Profile", 32,
                               window.getSize().x / 2.f - 100.f, 20.f, true,
                               Color::White);

                  float yPos = 100.f;
                  drawMenuText(window, font, "Username: " + username, 20, 40.f,
                               yPos, false, Color(220, 220, 220));
                  yPos += 35.f;

                  if (!currentPlayer->nickname.empty())
                  {
                    drawMenuText(window, font, "Nickname: " + nickname, 20,
                                 40.f, yPos, false, Color(220, 220, 220));
                    yPos += 35.f;
                  }

                  drawMenuText(window, font,
                               "Friends: " + to_string(friendCount), 20, 40.f,
                               yPos, false, Color(220, 220, 220));
                  yPos += 35.f;

                  drawMenuText(window, font,
                               "Total Points: " + to_string(totalPoints), 20,
                               40.f, yPos, false, Color(255, 200, 50));
                  yPos += 40.f;

                  drawMenuText(window, font, "Match History:", 20, 40.f, yPos,
                               true, Color::White);
                  yPos += 30.f;

                  // Show friends list
                  if (friendCount > 0)
                  {
                    drawMenuText(window, font, "Friends List:", 18, 40.f, yPos,
                                 false, Color(200, 200, 200));
                    yPos += 25.f;
                    int showFriends = friendCount < 8 ? friendCount : 8;
                    for (int i = 0; i < showFriends; ++i)
                    {
                      string friendName;
                      friendsSys.getFriendAt(username, i, friendName);
                      drawMenuText(window, font, "  - " + friendName, 16, 50.f,
                                   yPos, false, Color(180, 180, 180));
                      yPos += 22.f;
                    }
                  }

                  drawMenuText(window, font, "Press ESC or ENTER to return", 14,
                               40.f, yPos + 20.f, false, Color(180, 180, 180));
                  window.display();
                }
              }
            }
            else if (selected == 7)
            { // Leaderboard
              // Draw a simple leaderboard screen using the menu data
              bool showingLB = true;
              while (showingLB && window.isOpen())
              {
                Event ev2;
                while (window.pollEvent(ev2))
                {
                  if (ev2.type == Event::Closed)
                  {
                    window.close();
                    showingLB = false;
                    inStartMenu = false;
                    running = false;
                    break;
                  }
                  if (ev2.type == Event::KeyPressed)
                  {
                    if (ev2.key.code == Keyboard::Escape ||
                        ev2.key.code == Keyboard::Return)
                    {
                      showingLB = false;
                      break;
                    }
                  }
                }
                // Modern dark theme for leaderboard
                window.clear(Color(25, 30, 40));
                RectangleShape titleBar(Vector2f(window.getSize().x, 70.f));
                titleBar.setPosition(0, 0);
                titleBar.setFillColor(Color(40, 50, 70));
                window.draw(titleBar);
                drawMenuText(window, font, "Leaderboard (Top 10)", 32,
                             window.getSize().x / 2.f - 140.f, 20.f, true,
                             Color::White);

                int n = leaderboard.count();
                int shown = n < 10 ? n : 10;
                float startY = 100.f;

                // Header
                drawMenuText(window, font, "Rank    Player          Score", 18,
                             40.f, startY, true, Color(200, 200, 200));
                startY += 30.f;

                for (int i = 0; i < shown; ++i)
                {
                  string name;
                  int sc;
                  if (leaderboard.getEntry(i, name, sc))
                  {
                    Color rankColor =
                        (i < 3) ? Color(255, 200, 50) : Color::White;
                    string rankStr = to_string(i + 1);
                    if (rankStr.length() == 1)
                      rankStr = " " + rankStr;
                    string line = rankStr + ".  " + name;
                    // Pad name for alignment
                    while (line.length() < 20)
                      line += " ";
                    line += "  " + to_string(sc);
                    drawMenuText(window, font, line, 18, 40.f,
                                 startY + i * 28.f, false, rankColor);
                  }
                }
                drawMenuText(window, font, "Press ESC or ENTER to return", 14,
                             40.f, startY + shown * 28.f + 20.f, false,
                             Color(180, 180, 180));
                window.display();
              }
            }
            else if (selected == 8)
            { // Load Save
              string sid =
                  showTextInput(window, font, "Enter SaveID to load:", "", 64);
              if (!sid.empty())
              {
                SaveGame loaded;
                if (SaveGame::loadFromFile(sid, loaded))
                {
                  // restore basic game state
                  // reset playfield to borders
                  for (int i = 0; i < M; ++i)
                    for (int j = 0; j < N; ++j)
                      grid[i][j] =
                          (i == 0 || j == 0 || i == M - 1 || j == N - 1) ? 1
                                                                         : 0;

                  // fill tiles from save (iterate linked list)
                  for (TileNode *tn = loaded.getTiles(); tn; tn = tn->next)
                  {
                    if (tn->y >= 0 && tn->y < M && tn->x >= 0 && tn->x < N)
                      grid[tn->y][tn->x] = 1;
                  }

                  // restore player position/direction
                  playerX = loaded.playerX;
                  playerY = loaded.playerY;
                  dx = loaded.playerDX;
                  dy = loaded.playerDY;

                  // restore score system state
                  scoreSys.setScore(loaded.currentScore);
                  scoreSys.setInventory(loaded.powerupInventory);
                  scoreSys.setRewardCount(loaded.rewardCount);

                  // reset timer for game loop
                  timer = 0.f;
                  clock.restart();

                  // enter game with loaded state
                  inStartMenu = false;
                  inGame = true;
                  isLoadingSave = true; // mark that we're loading a save
                }
                else
                {
                  float ttime = 0.f;
                  Clock rtc;
                  while (ttime < 1.2f && window.isOpen())
                  {
                    Event fe;
                    while (window.pollEvent(fe))
                    {
                      if (fe.type == Event::Closed)
                      {
                        window.close();
                        running = false;
                        break;
                      }
                    }
                    window.clear(Color(18, 18, 28));
                    drawMenuText(window, font,
                                 string("Failed to load save: ") + sid, 18,
                                 20.f, 20.f, false);
                    window.display();
                    ttime = rtc.getElapsedTime().asSeconds();
                  }
                }
              }
            }
            else if (selected == 9)
            { // Logout -> go back to auth screen
              // Call auth UI again; it will set currentPlayer (or nullptr for
              // guest)
              if (!showAuthScreen(window, font, auth, currentPlayer))
              {
                // user closed window during auth
                window.close();
                running = false;
                inStartMenu = false;
                break;
              }
              // reload per-user inventory and theme
              if (currentPlayer)
              {
                friendsSys.ensurePlayer(currentPlayer->username);
                int inv = 0;
                if (loadUserInventory(currentPlayer->username, inv))
                  scoreSys.setInventory(inv);

                // Reload theme
                currentThemeId = "";
                inventory.loadUserTheme(currentPlayer->username,
                                        currentThemeId);
                if (!currentThemeId.empty())
                {
                  inventory.searchTheme(currentThemeId, currentTheme);
                  menuBgColor = getThemeBackgroundColor(currentTheme,
                                                        Color(135, 206, 250));
                }
                else
                {
                  menuBgColor = Color(135, 206, 250); // Default
                }
              }
              else
              {
                // Guest - use default theme
                currentThemeId = "";
                menuBgColor = Color(135, 206, 250);
              }
              // continue showing main menu after login
              inStartMenu = true;
            }
            else if (selected == 10)
            { // Exit
              window.close();
              running = false;
              inStartMenu = false;
            }
          }
        }

        // mouse click selection (simple)
        if (ev.type == Event::MouseButtonPressed &&
            ev.mouseButton.button == Mouse::Left)
        {
          Vector2i mp = Mouse::getPosition(window);
          int idx = (mp.y - 100) / 30; // adjust mapping as needed
          if (idx >= 0 && idx < OPTIONS)
          {
            selected = idx;
          }
        }
      } // event polling

      if (!window.isOpen() || !running)
        break;

      // Reload theme if needed (in case it changed)
      if (currentPlayer)
      {
        string tempThemeId = "";
        inventory.loadUserTheme(currentPlayer->username, tempThemeId);
        if (!tempThemeId.empty() && tempThemeId != currentThemeId)
        {
          currentThemeId = tempThemeId;
          inventory.searchTheme(currentThemeId, currentTheme);
          menuBgColor =
              getThemeBackgroundColor(currentTheme, Color(135, 206, 250));
        }
      }

      // Firewatch-style menu design - clean and minimalist
      window.clear(menuBgColor);

      // Game title - large and prominent (Firewatch style)
      float titleY = 50.f;
      drawMenuText(window, font, "XONIX", 64, window.getSize().x / 2.f - 120.f,
                   titleY, true, Color::White);

      // Menu options with scrolling support
      float menuX = window.getSize().x / 2.f - 100.f;
      float startY = 180.f;
      float itemSpacing = 40.f;
      int windowHeight = window.getSize().y;
      int maxVisibleItems = (windowHeight - (int)startY - 100) /
                            (int)itemSpacing; // Leave space for bottom info

      // Ensure at least 3 items are visible
      if (maxVisibleItems < 3)
        maxVisibleItems = 3;
      if (maxVisibleItems > OPTIONS)
        maxVisibleItems = OPTIONS;

      // Calculate scroll offset to keep selected item visible and centered when
      // possible
      int scrollOffset = 0;
      int centerOffset = maxVisibleItems / 2; // Try to center the selected item

      if (selected < centerOffset)
      {
        // Selected item is near the top, start from beginning
        scrollOffset = 0;
      }
      else if (selected >= OPTIONS - centerOffset)
      {
        // Selected item is near the bottom, show from end
        scrollOffset = OPTIONS - maxVisibleItems;
        if (scrollOffset < 0)
          scrollOffset = 0;
      }
      else
      {
        // Center the selected item
        scrollOffset = selected - centerOffset;
        if (scrollOffset < 0)
          scrollOffset = 0;
        if (scrollOffset + maxVisibleItems > OPTIONS)
        {
          scrollOffset = OPTIONS - maxVisibleItems;
          if (scrollOffset < 0)
            scrollOffset = 0;
        }
      }

      // Draw visible menu items
      int endIdx = scrollOffset + maxVisibleItems;
      if (endIdx > OPTIONS)
        endIdx = OPTIONS;

      for (int i = scrollOffset; i < endIdx; ++i)
      {
        bool sel = (i == selected);
        Color itemColor = sel ? Color(255, 215, 0)
                              : Color::White; // Golden yellow for selected
        int fontSize = sel ? 24 : 20;
        float yPos = startY + (i - scrollOffset) * itemSpacing;

        // Draw menu item
        drawMenuText(window, font, items[i], fontSize, menuX, yPos, sel,
                     itemColor);
      }

      // Draw scroll indicators if needed (subtle arrows)
      if (scrollOffset > 0)
      {
        // Up arrow indicator at top
        drawMenuText(window, font, "^", 16, menuX - 25.f, startY - 5.f, false,
                     Color(255, 255, 255, 120));
      }
      if (scrollOffset + maxVisibleItems < OPTIONS)
      {
        // Down arrow indicator at bottom
        float bottomY = startY + (endIdx - scrollOffset - 1) * itemSpacing;
        drawMenuText(window, font, "v", 16, menuX - 25.f,
                     bottomY + itemSpacing + 5.f, false,
                     Color(255, 255, 255, 120));
      }

      // User info at bottom left (subtle)
      if (currentPlayer)
      {
        string userDisplay = currentPlayer->nickname.empty()
                                 ? currentPlayer->username
                                 : currentPlayer->nickname;
        drawMenuText(window, font, "User: " + userDisplay, 14, 20.f,
                     window.getSize().y - 60.f, false,
                     Color(255, 255, 255, 200));
      }

      // Level info
      string levelDisplay = "Level: " + to_string(currentLevelNumber);
      drawMenuText(window, font, levelDisplay, 14, 20.f,
                   window.getSize().y - 40.f, false, Color(255, 255, 255, 200));

      window.display();
    } // end start menu loop

    // if user started single player, run the game loop once
    if (!window.isOpen() || !running)
      break;
    if (inGame)
    {
      // Only reset game state if NOT loading a saved game
      if (!isLoadingSave)
      {
        // reset game state
        for (int i = 0; i < M; ++i)
          for (int j = 0; j < N; ++j)
            grid[i][j] = (i == 0 || j == 0 || i == M - 1 || j == N - 1) ? 1 : 0;

        playerX = 10;
        playerY = 0;
        dx = dy = 0;
        timer = 0.f;
        clock.restart();

        // apply level settings: update enemy count and movement delay
        enemyCount = currentLevel.enemyCount;
        if (enemyCount < 1)
          enemyCount = 1;
        if (enemyCount > 20)
          enemyCount = 20;
        delay = currentLevel.delay;

        // initialize enemy positions and velocities for this run
        for (int i = 0; i < enemyCount; ++i)
        {
          // place enemies away from borders (2..N-3, 2..M-3 tiles) and center
          // in tile
          int ex = (rand() % (N - 4)) + 2;
          int ey = (rand() % (M - 4)) + 2;
          enemies[i].x = ex * ts + ts / 2;
          enemies[i].y = ey * ts + ts / 2;
          // random velocity 1..3 pixels per tick, random direction
          enemies[i].dx = (rand() % 3) + 1;
          if (rand() % 2)
            enemies[i].dx = -enemies[i].dx;
          enemies[i].dy = (rand() % 3) + 1;
          if (rand() % 2)
            enemies[i].dy = -enemies[i].dy;
        }

        // reset score system at start of each run
        scoreSys.reset(true);
      }
      else
      {
        // When loading a save, we still need to set delay and enemy count from
        // level
        delay = currentLevel.delay;
        enemyCount = currentLevel.enemyCount;
        if (enemyCount < 1)
          enemyCount = 1;
        if (enemyCount > 20)
          enemyCount = 20;

        // Reinitialize enemies (since we don't save enemy positions)
        // Place them in safe areas (not on filled tiles)
        for (int i = 0; i < enemyCount; ++i)
        {
          int attempts = 0;
          int ex, ey;
          do
          {
            ex = (rand() % (N - 4)) + 2;
            ey = (rand() % (M - 4)) + 2;
            attempts++;
          } while (grid[ey][ex] == 1 && attempts < 50); // avoid filled tiles

          enemies[i].x = ex * ts + ts / 2;
          enemies[i].y = ey * ts + ts / 2;
          enemies[i].dx = (rand() % 3) + 1;
          if (rand() % 2)
            enemies[i].dx = -enemies[i].dx;
          enemies[i].dy = (rand() % 3) + 1;
          if (rand() % 2)
            enemies[i].dy = -enemies[i].dy;
        }

        isLoadingSave = false; // reset flag after handling
      }

      bool gameOver = false;
      while (window.isOpen() && !gameOver)
      {
        float dt = clock.restart().asSeconds();
        timer += dt;

        // reset movement flag and remember previous position
        movedThisTick = false;
        prevPlayerX = playerX;
        prevPlayerY = playerY;

        // update score system timers
        scoreSys.update(dt);

        Event ev;
        while (window.pollEvent(ev))
        {
          if (ev.type == Event::Closed)
          {
            window.close();
            running = false;
            gameOver = true;
            break;
          }
          if (ev.type == Event::KeyPressed)
          {
            if (ev.key.code == Keyboard::Escape)
            {
              // simple pause menu implemented inline: Resume / Restart / Main
              // Menu / Exit
              int psel = 0;
              string popts[5] = {"Resume", "Restart", "Save", "Main Menu",
                                 "Exit"};
              bool inPause = true;
              while (inPause && window.isOpen())
              {
                Event ev2;
                while (window.pollEvent(ev2))
                {
                  if (ev2.type == Event::Closed)
                  {
                    window.close();
                    running = false;
                    inPause = false;
                    gameOver = true;
                    break;
                  }
                  if (ev2.type == Event::KeyPressed)
                  {
                    if (ev2.key.code == Keyboard::Up)
                      psel = (psel - 1 + 5) % 5;
                    if (ev2.key.code == Keyboard::Down)
                      psel = (psel + 1) % 5;
                    if (ev2.key.code == Keyboard::Return)
                    {
                      if (psel == 0)
                      {
                        inPause = false;
                      } // Resume
                      else if (psel == 1)
                      { // Restart
                        // reset level and break to restart loop
                        for (int i = 1; i < M - 1; ++i)
                          for (int j = 1; j < N - 1; ++j)
                            grid[i][j] = 0;
                        playerX = 10;
                        playerY = 0;
                        dx = dy = 0;
                        timer = 0.f;
                        inPause = false;
                      }
                      else if (psel == 2)
                      { // Save
                        // ask for SaveID (auto default)
                        string defaultId =
                            (currentPlayer ? currentPlayer->username
                                           : string("Guest")) +
                            string("_") + to_string((long long)time(0));
                        string sid = showTextInput(
                            window, font,
                            string("Enter SaveID (or accept default):"),
                            defaultId, 48);
                        if (!sid.empty())
                        {
                          SaveGame sg(sid, currentPlayer
                                               ? currentPlayer->username
                                               : string("Guest"));
                          sg.playerX = playerX;
                          sg.playerY = playerY;
                          sg.playerDX = dx;
                          sg.playerDY = dy;
                          // save score system state
                          sg.currentScore = scoreSys.getScore();
                          sg.powerupInventory = scoreSys.getInventory();
                          sg.rewardCount = scoreSys.getRewardCount();
                          // collect tiles that are filled (grid==1) and
                          // player's trail (2) as well
                          for (int yy = 0; yy < M; ++yy)
                            for (int xx = 0; xx < N; ++xx)
                            {
                              if (grid[yy][xx] == 1 || grid[yy][xx] == 2)
                                sg.addTile(xx, yy);
                            }
                          bool ok = sg.saveToFile();
                          float ttime = 0.f;
                          Clock rtc;
                          while (ttime < 1.2f && window.isOpen())
                          {
                            Event fe;
                            while (window.pollEvent(fe))
                            {
                              if (fe.type == Event::Closed)
                              {
                                window.close();
                                running = false;
                                break;
                              }
                            }
                            window.clear(Color(12, 12, 20));
                            drawMenuText(window, font,
                                         ok ? string("Game saved: ") + sid
                                            : string("Save failed"),
                                         18, 20.f, 20.f, false);
                            window.display();
                            ttime = rtc.getElapsedTime().asSeconds();
                          }
                        }
                      }
                      else if (psel == 3)
                      { // Main Menu
                        inPause = false;
                        gameOver = true;
                        break;
                      }
                      else if (psel == 4)
                      { // Exit
                        window.close();
                        running = false;
                        inPause = false;
                        gameOver = true;
                        break;
                      }
                    }
                  }
                }
                // draw pause menu
                window.clear(Color(10, 10, 20));
                drawMenuText(window, font, "Paused", 30, 20.f, 20.f, true);
                for (int i = 0; i < 5; ++i)
                {
                  bool sel = (i == psel);
                  drawMenuText(window, font, (sel ? "> " : "  ") + popts[i],
                               (sel ? 22 : 18), 40.f, 80.f + i * 32.f, sel);
                }
                window.display();
              } // end pause
            }
            if (ev.key.code == Keyboard::Space)
            {
              // try to use a power-up to freeze enemies
              if (scoreSys.usePowerup())
              {
                // persist updated inventory for logged-in user
                if (currentPlayer)
                  saveUserInventory(currentPlayer->username,
                                    scoreSys.getInventory());
              }
            }
          }
        }

        // controls
        if (Keyboard::isKeyPressed(Keyboard::Left))
        {
          dx = -1;
          dy = 0;
        }
        if (Keyboard::isKeyPressed(Keyboard::Right))
        {
          dx = 1;
          dy = 0;
        }
        if (Keyboard::isKeyPressed(Keyboard::Up))
        {
          dx = 0;
          dy = -1;
        }
        if (Keyboard::isKeyPressed(Keyboard::Down))
        {
          dx = 0;
          dy = 1;
        }

        if (timer > delay)
        {
          playerX += dx;
          playerY += dy;
          if (playerX < 0)
            playerX = 0;
          if (playerX > N - 1)
            playerX = N - 1;
          if (playerY < 0)
            playerY = 0;
          if (playerY > M - 1)
            playerY = M - 1;

          if (grid[playerY][playerX] == 2)
            gameOver = true;
          if (grid[playerY][playerX] == 0)
            grid[playerY][playerX] = 2;

          // detect whether a real movement happened this tick
          movedThisTick = (playerX != prevPlayerX || playerY != prevPlayerY);

          // If player moved to a non-filled tile, reset lastCaptured marker so
          // future captures allowed
          if (movedThisTick && grid[playerY][playerX] != 1)
          {
            lastCapturedX = -1;
            lastCapturedY = -1;
          }

          timer = 0.f;
        }

        // move enemies (skip movement when frozen)
        if (!scoreSys.isEnemiesFrozen())
        {
          for (int i = 0; i < enemyCount; ++i)
            enemies[i].move();
        }

        // only process capture when player has just moved into a filled tile
        // and not previously processed declare capturedTiles here so it's in
        // scope for the capture logic below
        int capturedTiles = 0;
        if (grid[playerY][playerX] == 1 && movedThisTick &&
            (playerX != lastCapturedX || playerY != lastCapturedY))
        {
          dx = dy = 0;
          // before filling captured area, compute how many tiles were captured
          capturedTiles = 0;
          // any cell that changed from 0 to 1 due to filling will count
          // We'll mark captured by running drop for enemies and then counting
          // -1 cells which later become 0 or 1.
          for (int i = 0; i < enemyCount; ++i)
          {
            drop(enemies[i].y / ts, enemies[i].x / ts);
          }
          // convert -1 to 0 and count captured tiles (cells that changed from 0
          // or 2 to 1)
          for (int i = 0; i < M; ++i)
          {
            for (int j = 0; j < N; ++j)
            {
              if (grid[i][j] == -1)
              {
                // area reachable by enemies remains empty
                grid[i][j] = 0;
              }
              else if (grid[i][j] == 0)
              {
                // this empty cell is now captured
                grid[i][j] = 1;
                capturedTiles++;
              }
              else if (grid[i][j] == 2)
              {
                // player's trail becomes filled and counts as captured
                grid[i][j] = 1;
                capturedTiles++;
              }
              else
              {
                // grid[i][j] == 1 -> already filled, leave as is
              }
            }
          }

          // register capture with score system only if something was captured
          if (capturedTiles > 0)
          {
            int points = scoreSys.registerCapture(capturedTiles);
            // persist updated inventory if logged-in
            if (currentPlayer)
              saveUserInventory(currentPlayer->username,
                                scoreSys.getInventory());
          }

          // mark this position as processed so we won't double-process
          lastCapturedX = playerX;
          lastCapturedY = playerY;
        }

        for (int i = 0; i < enemyCount; ++i)
          if (grid[enemies[i].y / ts][enemies[i].x / ts] == 2)
            gameOver = true;

        // draw
        window.clear();
        for (int i = 0; i < M; ++i)
        {
          for (int j = 0; j < N; ++j)
          {
            if (grid[i][j] == 0)
              continue;
            if (grid[i][j] == 1)
              sTile.setTextureRect(IntRect(0, 0, ts, ts));
            if (grid[i][j] == 2)
              sTile.setTextureRect(IntRect(54, 0, ts, ts));
            sTile.setPosition(j * ts, i * ts);
            window.draw(sTile);
          }
        }
        // player
        sTile.setTextureRect(IntRect(36, 0, ts, ts));
        sTile.setPosition(playerX * ts, playerY * ts);
        window.draw(sTile);

        // enemies
        sEnemy.rotate(10);
        for (int i = 0; i < enemyCount; ++i)
        {
          sEnemy.setPosition(enemies[i].x, enemies[i].y);
          window.draw(sEnemy);
        }

        // draw HUD: improved SFML HUD with semi-transparent background and
        // power-up icons
        {
          float winW = (float)window.getSize().x;
          RectangleShape hudBg(Vector2f(winW, 40.f));
          hudBg.setPosition(0.f, 0.f);
          hudBg.setFillColor(Color(8, 8, 16, 180));
          window.draw(hudBg);

          // score text
          Text tscore;
          tscore.setFont(font);
          tscore.setString(std::string("Score: ") +
                           std::to_string(scoreSys.getScore()));
          tscore.setCharacterSize(18);
          tscore.setFillColor(Color::White);
          tscore.setPosition(10.f, 8.f);
          window.draw(tscore);

          // powerup label
          Text tpuLabel;
          tpuLabel.setFont(font);
          tpuLabel.setString("Power-ups:");
          tpuLabel.setCharacterSize(14);
          tpuLabel.setFillColor(Color::White);
          tpuLabel.setPosition(200.f, 10.f);
          window.draw(tpuLabel);

          // draw small icons for each powerup (max 10 shown)
          int inv = scoreSys.getInventory();
          int maxShow = 10;
          int show = inv < maxShow ? inv : maxShow;
          for (int p = 0; p < show; ++p)
          {
            CircleShape ico(6.f);
            ico.setFillColor(Color(220, 200, 20));
            ico.setPosition(280.f + p * 18.f, 8.f);
            window.draw(ico);
          }
          if (inv > maxShow)
          {
            Text more;
            more.setFont(font);
            more.setString(std::string("x") + std::to_string(inv));
            more.setCharacterSize(14);
            more.setFillColor(Color::White);
            more.setPosition(280.f + maxShow * 18.f + 6.f, 8.f);
            window.draw(more);
          }

          // frozen state indicator
          if (scoreSys.isEnemiesFrozen())
          {
            Text tf;
            tf.setFont(font);
            tf.setString("Enemies Frozen");
            tf.setCharacterSize(14);
            tf.setFillColor(Color(150, 220, 255));
            tf.setPosition(winW - 160.f, 10.f);
            window.draw(tf);
          }

          // hint
          Text hint;
          hint.setFont(font);
          hint.setString("Press SPACE to use power-up");
          hint.setCharacterSize(12);
          hint.setFillColor(Color(180, 180, 180));
          hint.setPosition(10.f, 22.f);
          window.draw(hint);
        }

        window.display();
      } // end game loop

      // when gameOver, compute score and update leaderboard via GameMenu
      if (window.isOpen() && running)
      {
        int score = scoreSys.getScore();
        for (int i = 0; i < M; ++i)
          for (int j = 0; j < N; ++j)
            if (grid[i][j] == 1)
              ++score; // keep original tile count bonus

        bool newHigh = leaderboard.isNewHighScore(score);
        if (newHigh)
        {
          // if logged in use account name, else ask console
          string pname;
          if (currentPlayer)
          {
            if (!currentPlayer->nickname.empty())
              pname = currentPlayer->nickname;
            else
              pname = currentPlayer->username;
          }
          else
          {
            // Show on-screen input dialog for name entry (no console I/O)
            string entered = showTextInput(
                window, font,
                "New High Score! Enter your name (no spaces):", "Player", 16);
            if (entered.empty())
              entered = "Player";
            // sanitize spaces
            for (size_t i = 0; i < entered.size(); ++i)
              if (entered[i] == ' ')
                entered[i] = '_';
            pname = entered;
          }
          leaderboard.addScore(pname, score);
        }
        else
        {
          // still show end screen (no name entry)
        }

        // show a simple end menu on screen (Restart / Main Menu / Exit)
        int eSel = 0;
        string eopts[3] = {"Restart", "Main Menu", "Exit"};
        bool inEnd = true;
        while (inEnd && window.isOpen())
        {
          Event ev;
          while (window.pollEvent(ev))
          {
            if (ev.type == Event::Closed)
            {
              window.close();
              running = false;
              inEnd = false;
              break;
            }
            if (ev.type == Event::KeyPressed)
            {
              if (ev.key.code == Keyboard::Up)
                eSel = (eSel - 1 + 3) % 3;
              if (ev.key.code == Keyboard::Down)
                eSel = (eSel + 1) % 3;
              if (ev.key.code == Keyboard::Return)
              {
                if (eSel == 0)
                { // Restart -> re-run game loop
                  inEnd = false;
                  inGame = true;
                  break;
                }
                else if (eSel == 1)
                { // Main Menu
                  inEnd = false;
                  inGame = false;
                  break;
                }
                else
                { // Exit
                  window.close();
                  running = false;
                  inEnd = false;
                  break;
                }
              }
            }
          }
          window.clear(Color(20, 20, 20));
          string title = "Game Over - Score: " + to_string(score) +
                         (newHigh ? " (NEW HIGH!)" : "");
          drawMenuText(window, font, title, 26, 20.f, 20.f, true);
          for (int i = 0; i < 3; ++i)
          {
            bool sel = (i == eSel);
            drawMenuText(window, font, (sel ? "> " : "  ") + eopts[i],
                         (sel ? 22 : 18), 40.f, 80.f + i * 30.f, sel);
          }
          window.display();
        }
      }
    } // end inGame block

  } // end main while

  return 0;
}
