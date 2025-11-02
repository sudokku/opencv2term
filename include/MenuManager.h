#ifndef MENU_MANAGER_H
#define MENU_MANAGER_H

#include <string>
#include <vector>
#include "AsciiPalette.h"

class MenuManager {
public:
    MenuManager();
    ~MenuManager();
    
    // Initialize ncurses
    void initialize();
    
    // Cleanup ncurses
    void cleanup();
    
    // Show palette selection menu
    int selectPalette(const std::vector<AsciiPalette>& palettes);
    
    // Show image selection menu
    std::string selectImage(const std::vector<std::string>& images);
    
    // Show display mode selection menu
    int selectDisplayMode();
    
    // Show media type selection menu (image, video, or camera)
    int selectMediaType();

    // Show video selection menu
    std::string selectVideo(const std::vector<std::string> &videos);
    
    // Show color mode selection menu
    bool selectColorMode();
    
private:
    bool initialized_;
    
    // Generic menu implementation
    int showMenu(const std::string& title, const std::vector<std::string>& options, 
                 const std::vector<std::string>& descriptions = {});
};

#endif // MENU_MANAGER_H

