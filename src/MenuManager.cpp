#include "MenuManager.h"
#include <ncurses.h>
#include <algorithm>

MenuManager::MenuManager() : initialized_(false) {}

MenuManager::~MenuManager() {
    cleanup();
}

void MenuManager::initialize() {
    if (!initialized_) {
        initscr();
        noecho();
        curs_set(FALSE);
        initialized_ = true;
    }
}

void MenuManager::cleanup() {
    if (initialized_) {
        endwin();
        initialized_ = false;
    }
}

int MenuManager::selectPalette(const std::vector<AsciiPalette>& palettes) {
    std::vector<std::string> options;
    std::vector<std::string> descriptions;
    
    for (const auto& palette : palettes) {
        options.push_back(palette.getName());
        std::string preview = "  Preview: " + 
            palette.getCharacters().substr(0, std::min(40, (int)palette.getCharacters().length()));
        descriptions.push_back(preview);
    }
    
    return showMenu("Select an ASCII palette:", options, descriptions);
}

std::string MenuManager::selectImage(const std::vector<std::string>& images) {
    std::vector<std::string> options(images.begin(), images.end());
    int selected = showMenu("Select an image to display:", options);
    
    if (selected >= 0 && selected < (int)images.size()) {
        return images[selected];
    }
    return "";
}

int MenuManager::selectDisplayMode() {
    std::vector<std::string> options = {
        "Display in current terminal (interactive, resizable)",
        "Open in new window (static, with calculated dimensions)"
    };
    
    return showMenu("Select display mode:", options);
}

int MenuManager::showMenu(const std::string& title, const std::vector<std::string>& options,
                          const std::vector<std::string>& descriptions) {
    if (options.empty()) return -1;
    
    int selectedIndex = 0;
    int key;
    
    keypad(stdscr, TRUE);
    
    while (true) {
        clear();
        
        // Print title
        attron(A_BOLD);
        mvprintw(1, 2, "%s", title.c_str());
        attroff(A_BOLD);
        
        mvprintw(2, 2, "Use UP/DOWN arrows to navigate, ENTER to select, Q to quit");
        
        // Print options
        int lineOffset = 4;
        for (size_t i = 0; i < options.size(); i++) {
            if (i == (size_t)selectedIndex) {
                attron(A_REVERSE);
                mvprintw(lineOffset, 4, "> %s", options[i].c_str());
                attroff(A_REVERSE);
            } else {
                mvprintw(lineOffset, 4, "  %s", options[i].c_str());
            }
            lineOffset++;
            
            // Show description if available
            if (i < descriptions.size() && !descriptions[i].empty()) {
                mvprintw(lineOffset, 6, "%s", descriptions[i].c_str());
                lineOffset++;
            }
        }
        
        refresh();
        
        // Get user input
        key = getch();
        
        switch (key) {
            case KEY_UP:
                if (selectedIndex > 0) {
                    selectedIndex--;
                }
                break;
                
            case KEY_DOWN:
                if (selectedIndex < (int)options.size() - 1) {
                    selectedIndex++;
                }
                break;
                
            case '\n':
            case KEY_ENTER:
                return selectedIndex;
                
            case 'q':
            case 'Q':
                return -1;
        }
    }
}

