#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <opencv2/opencv.hpp>
#include <string>
#include "AsciiRenderer.h"

class DisplayManager {
public:
    DisplayManager(const AsciiRenderer* renderer);
    ~DisplayManager();
    
    // Display in current terminal (interactive mode)
    void displayInTerminal(const cv::Mat& image);
    
    // Open in new window (static mode)
    void displayInNewWindow(const cv::Mat& image, int maxWidth = 120, int maxHeight = 60);
    
private:
    const AsciiRenderer* renderer_;
    cv::Mat currentImage_;
    bool isRunning_;
    
    // Print image using ncurses
    void printImage(const cv::Mat& image);
    
    // Signal handler for window resize
    static void handleResizeSignal(int sig);
    
    // Static instance for signal handler
    static DisplayManager* instance_;
};

#endif // DISPLAY_MANAGER_H

