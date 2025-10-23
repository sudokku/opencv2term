#include "DisplayManager.h"
#include "ImageProcessor.h"
#include <ncurses.h>
#include <csignal>
#include <unistd.h>
#include <cstdlib>

// Initialize static member
DisplayManager* DisplayManager::instance_ = nullptr;

DisplayManager::DisplayManager(const AsciiRenderer* renderer)
    : renderer_(renderer), isRunning_(false) {
    instance_ = this;
}

DisplayManager::~DisplayManager() {
    if (isRunning_) {
        isRunning_ = false;
        signal(SIGWINCH, SIG_DFL);
        endwin();
    }
    instance_ = nullptr;
}

void DisplayManager::displayInTerminal(const cv::Mat& image) {
    if (image.empty() || !renderer_) return;
    
    currentImage_ = image;
    
    // Initialize ncurses
    initscr();
    noecho();
    curs_set(FALSE);
    
    // Set up resize signal handler
    signal(SIGWINCH, handleResizeSignal);
    
    isRunning_ = true;
    
    // Initial drawing
    printImage(currentImage_);
    
    // Main loop - wait for input
    while (isRunning_) {
        char c = getch();
        if (c == '\n') {  // Exit on ENTER
            break;
        }
    }
    
    // Cleanup
    isRunning_ = false;
    signal(SIGWINCH, SIG_DFL);
    endwin();
}

void DisplayManager::displayInNewWindow(const cv::Mat& image, int maxWidth, int maxHeight) {
    if (image.empty() || !renderer_) return;
    
    // Use ImageProcessor to resize with proper aspect ratio
    ImageProcessor processor;
    // Create temporary processor with the image
    cv::Mat tempImage = image.clone();
    
    // Calculate dimensions with aspect ratio
    double aspectRatio = (double)tempImage.cols / tempImage.rows;
    aspectRatio *= 2.0;  // Account for character aspect ratio
    
    int width, height;
    if (aspectRatio > ((double)maxWidth / maxHeight)) {
        width = maxWidth;
        height = (int)(width / aspectRatio);
    } else {
        height = maxHeight;
        width = (int)(height * aspectRatio);
    }
    
    if (width < 10) width = 10;
    if (height < 10) height = 10;
    
    cv::Mat resized;
    cv::resize(tempImage, resized, cv::Size(width, height));
    
    // Generate ASCII art to temporary file
    std::string tempFile = "/tmp/opencv2term_output.txt";
    if (renderer_->saveToFile(resized, tempFile)) {
        // Open in new terminal window (macOS)
        std::string command = "osascript -e 'tell application \"Terminal\" to do script \"clear && cat " + 
                             tempFile + " && echo \\\"\\\\nPress any key to close...\\\" && read -n 1\"'";
        system(command.c_str());
        
        sleep(1);  // Wait for window to open
    }
}

void DisplayManager::printImage(const cv::Mat& image) {
    if (image.empty() || !renderer_) return;
    
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    
    // Resize image to fit terminal
    cv::Mat resized;
    cv::resize(image, resized, cv::Size(max_x, max_y));
    
    // Create ASCII matrix
    char ascii_matrix[resized.rows * resized.cols];
    renderer_->renderToMatrix(resized, ascii_matrix);
    
    // Print to terminal
    clear();
    for (int i = 0; i < resized.rows; i++) {
        for (int j = 0; j < resized.cols; j++) {
            mvprintw(i, j, "%c", ascii_matrix[i * resized.cols + j]);
        }
    }
    refresh();
}

void DisplayManager::handleResizeSignal(int sig) {
    if (instance_ && instance_->isRunning_) {
        // Update ncurses to recognize new terminal size
        endwin();
        refresh();
        
        // Redraw image
        instance_->printImage(instance_->currentImage_);
    }
}

