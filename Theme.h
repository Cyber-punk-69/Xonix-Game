#ifndef THEME_H
#define THEME_H

#include <string>

using namespace std;

struct Theme
{
    string themeId;     // unique identifier (used as key in AVL tree)
    string themeName;   // display name
    string description; // short description
    string colorCode;   // color representation (e.g., "RGB(255,0,0)" or hex code)
    string bgImagePath; // background image path (optional, empty if using color
                        // only)

    Theme()
        : themeId(""), themeName(""), description(""), colorCode(""),
          bgImagePath("") {}
    Theme(const string &id, const string &name, const string &desc,
          const string &color, const string &bgImg = "")
        : themeId(id), themeName(name), description(desc), colorCode(color),
          bgImagePath(bgImg) {}
};

#endif // THEME_H
