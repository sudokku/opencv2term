#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <opencv2/opencv.hpp>
#include <string>
#include <functional>
#include "AsciiRenderer.h"

class DisplayManager {
public:
    DisplayManager(const AsciiRenderer* renderer);
    ~DisplayManager();
    
    // Display in current terminal (interactive mode)
    void displayInTerminal(const cv::Mat& image);

    // Display video in current terminal (interactive mode)
    // frameProvider should return false when video ends
    void displayVideoInTerminal(std::function<bool(cv::Mat &)> frameProvider, int paletteSize, double fps);

    // Open image in new window (static mode, clean display)
    void displayImageInNewWindow(const cv::Mat &image, int maxWidth = 120, int maxHeight = 60);

    // Open video in new window (static mode, clean display)
    void displayVideoInNewWindow(std::function<bool(cv::Mat &)> frameProvider,
                                 int paletteSize, double fps,
                                 int maxWidth = 120, int maxHeight = 60);

private:
    const AsciiRenderer* renderer_;
    cv::Mat currentImage_;
    bool isRunning_;
    
    // Print image using ncurses
    void printImage(const cv::Mat& image);

    // Generate shell script for clean external window display
    std::string generateDisplayScript(const std::string &asciiFile, bool isVideo = false);

    // Signal handler for window resize
    static void handleResizeSignal(int sig);
    
    // Static instance for signal handler
    static DisplayManager* instance_;
};

#endif // DISPLAY_MANAGER_H

