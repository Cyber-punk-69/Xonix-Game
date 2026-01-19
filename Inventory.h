#ifndef INVENTORY_H
#define INVENTORY_H

#include "AVLTree.h"
#include <string>

class Inventory {
public:
    Inventory();
    ~Inventory();
    
    // Add a theme to the inventory
    void addTheme(const Theme &theme);
    
    // Search for a theme by ID
    bool searchTheme(const std::string &themeId, Theme &out) const;
    
    // Get all themes in sorted order (returns number of themes)
    int getAllThemes(Theme *arr, int maxSize) const;
    
    // Load default themes (predefined set)
    void loadDefaultThemes();
    
    // Save user's theme preference to file
    bool saveUserTheme(const std::string &username, const std::string &themeId) const;
    
    // Load user's theme preference from file
    bool loadUserTheme(const std::string &username, std::string &themeId) const;
    
    // Get the number of themes
    int getThemeCount() const;
    
    // Clear all themes
    void clear();

private:
    AVLTree themeTree;
    static const int MAX_THEMES = 100; // maximum themes that can be stored
};

#endif // INVENTORY_H

