#include "DisplayManager.h"
#include "ImageProcessor.h"
#include <ncurses.h>
#include <csignal>
#include <unistd.h>
#include <cstdlib>
#include <fstream>
#include <chrono>
#include <thread>

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
    
    // Initialize colors if renderer uses color
    if (renderer_->isUsingColor() && has_colors()) {
        start_color();
        use_default_colors();
        
        // Initialize 16 color pairs (1-16) for basic terminal colors
        for (int i = 0; i < 16; i++) {
            init_pair(i + 1, i, -1);  // Foreground color i, default background
        }
    }
    
    // Set up resize signal handler
    signal(SIGWINCH, handleResizeSignal);
    
    isRunning_ = true;
    
    // Initial drawing
    printImage(currentImage_);
    
    // Main loop - wait for input
    while (isRunning_) {
        char c = getch();
        if (c == '\n' || c == 'q' || c == 'Q') { // Exit on ENTER or Q
            break;
        }
    }
    
    // Cleanup
    isRunning_ = false;
    signal(SIGWINCH, SIG_DFL);
    endwin();
}

void DisplayManager::displayVideoInTerminal(std::function<bool(cv::Mat &)> frameProvider,
                                            int paletteSize, double fps)
{
    if (!renderer_)
        return;

    // Initialize ncurses
    initscr();
    noecho();
    curs_set(FALSE);
    nodelay(stdscr, TRUE); // Non-blocking getch()
    
    // Initialize colors if renderer uses color
    if (renderer_->isUsingColor() && has_colors()) {
        start_color();
        use_default_colors();
        
        // Initialize 16 color pairs (1-16) for basic terminal colors
        for (int i = 0; i < 16; i++) {
            init_pair(i + 1, i, -1);  // Foreground color i, default background
        }
    }

    isRunning_ = true;

    // Calculate frame delay in microseconds
    int frameDelay = (int)(1000000.0 / fps);

    cv::Mat frame;
    while (isRunning_)
    {
        auto frameStart = std::chrono::high_resolution_clock::now();

        // Get next frame
        if (!frameProvider(frame))
        {
            break; // Video ended
        }

        // Clamp pixel values to palette size
        cv::Mat clampedFrame = frame.clone();
        for (int i = 0; i < clampedFrame.rows; i++)
        {
            for (int j = 0; j < clampedFrame.cols; j++)
            {
                clampedFrame.at<uchar>(i, j) = cv::saturate_cast<uchar>(
                    (paletteSize - 1) * (frame.at<uchar>(i, j) / 255.0));
            }
        }

        // Display frame
        printImage(clampedFrame);

        // Check for user input to quit
        char c = getch();
        if (c == 'q' || c == 'Q' || c == '\n')
        {
            break;
        }

        // Wait for correct frame timing
        auto frameEnd = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(frameEnd - frameStart).count();
        int remainingDelay = frameDelay - elapsed;
        if (remainingDelay > 0)
        {
            usleep(remainingDelay);
        }
    }

    // Cleanup
    isRunning_ = false;
    endwin();
}

void DisplayManager::displayImageInNewWindow(const cv::Mat &image, int maxWidth, int maxHeight)
{
    if (image.empty() || !renderer_)
        return;

    // Calculate dimensions with aspect ratio
    double aspectRatio = (double)image.cols / image.rows;
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
    cv::resize(image, resized, cv::Size(width, height));

    // Generate ASCII art to temporary file
    std::string tempFile = "/tmp/opencv2term_output.txt";
    if (renderer_->saveToFile(resized, tempFile)) {
        // Generate and execute display script
        std::string scriptFile = "/tmp/opencv2term_display.sh";
        std::string script = generateDisplayScript(tempFile, false);

        std::ofstream scriptOut(scriptFile);
        scriptOut << script;
        scriptOut.close();

        system(("chmod +x " + scriptFile).c_str());
        system(("open -a Terminal " + scriptFile).c_str());

        sleep(1);  // Wait for window to open
    }
}

void DisplayManager::displayVideoInNewWindow(std::function<bool(cv::Mat &)> frameProvider,
                                             int paletteSize, double fps,
                                             int maxWidth, int maxHeight)
{
    if (!renderer_)
        return;

    // Generate all frames to a temporary directory
    system("rm -rf /tmp/opencv2term_frames && mkdir -p /tmp/opencv2term_frames");

    int frameCount = 0;
    cv::Mat frame;

    // Calculate dimensions from first frame
    if (!frameProvider(frame))
        return;

    double aspectRatio = (double)frame.cols / frame.rows;
    aspectRatio *= 2.0;

    int width, height;
    if (aspectRatio > ((double)maxWidth / maxHeight))
    {
        width = maxWidth;
        height = (int)(width / aspectRatio);
    }
    else
    {
        height = maxHeight;
        width = (int)(height * aspectRatio);
    }

    if (width < 10)
        width = 10;
    if (height < 10)
        height = 10;

    // Process first frame
    cv::Mat clampedFrame = frame.clone();
    for (int i = 0; i < clampedFrame.rows; i++)
    {
        for (int j = 0; j < clampedFrame.cols; j++)
        {
            clampedFrame.at<uchar>(i, j) = cv::saturate_cast<uchar>(
                (paletteSize - 1) * (frame.at<uchar>(i, j) / 255.0));
        }
    }

    cv::Mat resized;
    cv::resize(clampedFrame, resized, cv::Size(width, height));

    char filename[256];
    snprintf(filename, sizeof(filename), "/tmp/opencv2term_frames/frame_%05d.txt", frameCount++);
    renderer_->saveToFile(resized, filename);

    // Process remaining frames
    while (frameProvider(frame))
    {
        clampedFrame = frame.clone();
        for (int i = 0; i < clampedFrame.rows; i++)
        {
            for (int j = 0; j < clampedFrame.cols; j++)
            {
                clampedFrame.at<uchar>(i, j) = cv::saturate_cast<uchar>(
                    (paletteSize - 1) * (frame.at<uchar>(i, j) / 255.0));
            }
        }

        cv::resize(clampedFrame, resized, cv::Size(width, height));

        snprintf(filename, sizeof(filename), "/tmp/opencv2term_frames/frame_%05d.txt", frameCount++);
        renderer_->saveToFile(resized, filename);
    }

    // Generate playback script
    std::string scriptFile = "/tmp/opencv2term_play.sh";
    std::ofstream script(scriptFile);

    double frameDelay = 1.0 / fps;

    script << "#!/bin/bash\n";
    script << "clear\n";
    script << "tput civis\n";                       // Hide cursor
    script << "trap 'tput cnorm; exit' INT TERM\n"; // Restore cursor on exit
    script << "\n";
    script << "while true; do\n";
    script << "  for frame in /tmp/opencv2term_frames/frame_*.txt; do\n";
    script << "    clear\n";
    script << "    cat \"$frame\"\n";
    script << "    sleep " << frameDelay << "\n";
    script << "  done\n";
    script << "done\n";

    script.close();

    system(("chmod +x " + scriptFile).c_str());
    system(("open -a Terminal " + scriptFile).c_str());

    sleep(1);
}

std::string DisplayManager::generateDisplayScript(const std::string &asciiFile, bool isVideo)
{
    std::ostringstream script;

    script << "#!/bin/bash\n";
    script << "clear\n";
    script << "tput civis\n";                       // Hide cursor
    script << "trap 'tput cnorm; exit' INT TERM\n"; // Restore cursor on exit
    script << "\n";

    if (isVideo)
    {
        script << "# Video playback\n";
        script << "cat \"" << asciiFile << "\"\n";
    }
    else
    {
        script << "# Static image display\n";
        script << "cat \"" << asciiFile << "\"\n";
        script << "echo \"\"\n";
        script << "read -n 1 -s -r -p \"Press any key to close...\"\n";
        script << "tput cnorm\n"; // Restore cursor
    }

    return script.str();
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

void DisplayManager::printImageWithColor(const cv::Mat& grayImage, const cv::Mat& colorImage) {
    if (grayImage.empty() || colorImage.empty() || !renderer_) return;
    
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    
    // Resize both images to fit terminal
    cv::Mat grayResized, colorResized;
    cv::resize(grayImage, grayResized, cv::Size(max_x, max_y));
    cv::resize(colorImage, colorResized, cv::Size(max_x, max_y));
    
    // Create ASCII matrix with color information
    int size = grayResized.rows * grayResized.cols;
    char ascii_matrix[size];
    int color_pairs[size];
    renderer_->renderToMatrixWithColor(colorResized, grayResized, ascii_matrix, color_pairs);
    
    // Print to terminal with colors
    clear();
    for (int i = 0; i < grayResized.rows; i++) {
        for (int j = 0; j < grayResized.cols; j++) {
            int idx = i * grayResized.cols + j;
            
            // Apply color attribute
            attron(COLOR_PAIR(color_pairs[idx] + 1));
            mvaddch(i, j, ascii_matrix[idx]);
            attroff(COLOR_PAIR(color_pairs[idx] + 1));
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
