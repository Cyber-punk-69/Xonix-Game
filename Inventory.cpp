#include "Inventory.h"
#include <cstdio>
#include <cstring>

Inventory::Inventory()
{
    loadDefaultThemes();
}

Inventory::~Inventory()
{
}

void Inventory::addTheme(const Theme &theme)
{
    themeTree.insert(theme);
}

bool Inventory::searchTheme(const std::string &themeId, Theme &out) const
{
    return themeTree.search(themeId, out);
}

int Inventory::getAllThemes(Theme *arr, int maxSize) const
{
    return themeTree.inOrderTraversal(arr, maxSize);
}

void Inventory::loadDefaultThemes()
{
    // Themes with background colors - can add image paths later if needed
    addTheme(Theme("T001", "Classic Blue", "Traditional blue color scheme", "RGB(50,100,200)", ""));
    addTheme(Theme("T002", "Forest Green", "Natural green forest theme", "RGB(34,139,34)", ""));
    addTheme(Theme("T003", "Sunset Orange", "Warm orange sunset colors", "RGB(255,140,0)", ""));
    addTheme(Theme("T004", "Midnight Purple", "Dark purple night theme", "RGB(75,0,130)", ""));
    addTheme(Theme("T005", "Ocean Teal", "Cool ocean teal colors", "RGB(0,128,128)", ""));
    addTheme(Theme("T006", "Crimson Red", "Bold red color scheme", "RGB(220,20,60)", ""));
    addTheme(Theme("T007", "Golden Yellow", "Bright golden yellow theme", "RGB(255,215,0)", ""));
    addTheme(Theme("T008", "Neon Pink", "Vibrant neon pink theme", "RGB(255,20,147)", ""));
    // Default theme with Firewatch-style colors
    addTheme(Theme("T000", "Firewatch", "Clean minimalist theme", "RGB(135,206,250)", ""));
}

bool Inventory::saveUserTheme(const std::string &username, const std::string &themeId) const
{
    const int MAX_LINES = 1024;
    char lines[MAX_LINES][256];
    int count = 0;

    FILE *f = fopen("themes.txt", "r");
    if (f)
    {
        while (count < MAX_LINES && fgets(lines[count], sizeof(lines[count]), f))
        {
            count++;
        }
        fclose(f);
    }

    bool updated = false;
    f = fopen("themes.txt", "w");
    if (!f)
        return false;

    for (int i = 0; i < count; ++i)
    {
        char u[128];
        char tid[64];
        if (sscanf(lines[i], "%127s %63s", u, tid) == 2)
        {
            if (username == std::string(u))
            {
                fprintf(f, "%s %s\n", username.c_str(), themeId.c_str());
                updated = true;
            }
            else
            {
                fprintf(f, "%s", lines[i]);
            }
        }
    }

    if (!updated)
    {
        fprintf(f, "%s %s\n", username.c_str(), themeId.c_str());
    }

    fclose(f);
    return true;
}

bool Inventory::loadUserTheme(const std::string &username, std::string &themeId) const
{
    FILE *f = fopen("themes.txt", "r");
    if (!f)
        return false;

    char buf[256];
    while (fgets(buf, sizeof(buf), f))
    {
        char u[128];
        char tid[64];
        if (sscanf(buf, "%127s %63s", u, tid) == 2)
        {
            if (username == std::string(u))
            {
                themeId = std::string(tid);
                fclose(f);
                return true;
            }
        }
    }

    fclose(f);
    return false;
}

int Inventory::getThemeCount() const
{
    return themeTree.size();
}

void Inventory::clear()
{
    themeTree.clear();
}
