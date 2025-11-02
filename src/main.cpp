#include <iostream>
#include <unistd.h>
#include "AsciiPalette.h"
#include "ImageProcessor.h"
#include "VideoProcessor.h"
#include "CameraProcessor.h"
#include "AsciiRenderer.h"
#include "DisplayManager.h"
#include "MenuManager.h"
#include <ncurses.h>

int main(int argc, char** argv) {
    // Configuration
    const std::string imagesDir = "../images";
    const int maxWindowWidth = 120;
    const int maxWindowHeight = 60;

    // Get available images and videos
    auto imageFiles = ImageProcessor::getImageFiles(imagesDir);
    auto videoFiles = VideoProcessor::getVideoFiles(imagesDir);

    if (imageFiles.empty() && videoFiles.empty())
    {
        std::cerr << "No media files found in " << imagesDir << " directory" << std::endl;
        std::cerr << "Please add some image files (.jpg, .png, .bmp, etc.) or video files (.mp4, .avi, .mov, etc.)" << std::endl;
        return -1;
    }

    // Get available palettes
    auto palettes = AsciiPalette::getDefaultPalettes();
    
    // Initialize menu manager
    MenuManager menuManager;
    menuManager.initialize();
    
    // Step 1: Select palette
    int paletteIndex = menuManager.selectPalette(palettes);
    if (paletteIndex < 0) {
        menuManager.cleanup();
        std::cout << "No palette selected. Exiting..." << std::endl;
        return 0;
    }
    const AsciiPalette& selectedPalette = palettes[paletteIndex];

    // Step 2: Select color mode
    bool useColor = menuManager.selectColorMode();

    // Step 3: Select media type (image, video, or camera)
    int mediaType = menuManager.selectMediaType();
    if (mediaType < 0)
    {
        menuManager.cleanup();
        std::cout << "No media type selected. Exiting..." << std::endl;
        return 0;
    }

    std::string selectedMedia;
    bool isVideo = (mediaType == 1);
    bool isCamera = (mediaType == 2);

    // Step 3: Select specific media file (skip for camera)
    if (isCamera) {
        // Camera doesn't need file selection
        selectedMedia = "";
    }
    else if (isVideo)
    {
        if (videoFiles.empty())
        {
            menuManager.cleanup();
            std::cerr << "No video files found in " << imagesDir << " directory" << std::endl;
            return -1;
        }
        selectedMedia = menuManager.selectVideo(videoFiles);
    }
    else
    {
        if (imageFiles.empty())
        {
            menuManager.cleanup();
            std::cerr << "No image files found in " << imagesDir << " directory" << std::endl;
            return -1;
        }
        selectedMedia = menuManager.selectImage(imageFiles);
    }

    if (selectedMedia.empty() && !isCamera)
    {
        menuManager.cleanup();
        std::cout << "No media selected. Exiting..." << std::endl;
        return 0;
    }

    // Step 4: Select display mode
    int displayMode = menuManager.selectDisplayMode();
    if (displayMode < 0) {
        menuManager.cleanup();
        std::cout << "No display mode selected. Exiting..." << std::endl;
        return 0;
    }
    
    // Clean up menu UI
    menuManager.cleanup();

    std::string mediaPath = imagesDir + "/" + selectedMedia;

    // Create renderer with selected palette and color mode
    AsciiRenderer renderer(&selectedPalette, useColor);
    DisplayManager displayManager(&renderer);

    if (isCamera)
    {
        // Handle live camera feed
        CameraProcessor cameraProcessor;

        std::cout << "Opening camera..." << std::endl;
        if (!cameraProcessor.openCamera(0))  // 0 = default camera
        {
            std::cerr << "Error: Could not open camera. Please check:" << std::endl;
            std::cerr << "  - Camera is connected" << std::endl;
            std::cerr << "  - No other app is using the camera" << std::endl;
            std::cerr << "  - Camera permissions are granted" << std::endl;
            return -1;
        }

        std::cout << "Camera opened successfully!" << std::endl;
        std::cout << "  FPS: " << cameraProcessor.getFPS() << std::endl;
        std::cout << "  Resolution: " << cameraProcessor.getWidth() << "x" << cameraProcessor.getHeight() << std::endl;

        if (displayMode == 0)
        {
            // Display in current terminal
            std::cout << "Starting live camera feed... Press Q or ENTER to stop." << std::endl;
            sleep(2);

            auto frameProvider = [&cameraProcessor](cv::Mat &frame) -> bool
            {
                return cameraProcessor.getNextFrame(frame);
            };

            displayManager.displayVideoInTerminal(frameProvider, selectedPalette.getSize(),
                                                  cameraProcessor.getFPS());
        }
        else
        {
            // Camera in new window not supported - use current terminal
            std::cout << "Note: Live camera feed only supports current terminal display." << std::endl;
            std::cout << "Starting live camera feed... Press Q or ENTER to stop." << std::endl;
            sleep(2);

            auto frameProvider = [&cameraProcessor](cv::Mat &frame) -> bool
            {
                return cameraProcessor.getNextFrame(frame);
            };

            displayManager.displayVideoInTerminal(frameProvider, selectedPalette.getSize(),
                                                  cameraProcessor.getFPS());
        }

        cameraProcessor.release();
    }
    else if (isVideo)
    {
        // Handle video playback
        VideoProcessor videoProcessor;

        if (!videoProcessor.loadVideo(mediaPath))
        {
            std::cerr << "Error: Could not load video file: " << mediaPath << std::endl;
            return -1;
        }

        std::cout << "Video loaded: " << selectedMedia << std::endl;
        std::cout << "  Frames: " << videoProcessor.getFrameCount() << std::endl;
        std::cout << "  FPS: " << videoProcessor.getFPS() << std::endl;
        std::cout << "  Resolution: " << videoProcessor.getWidth() << "x" << videoProcessor.getHeight() << std::endl;

        if (displayMode == 0)
        {
            // Display in current terminal
            std::cout << "Playing video... Press Q or ENTER to stop." << std::endl;
            sleep(2);

            auto frameProvider = [&videoProcessor, &selectedPalette](cv::Mat &frame) -> bool
            {
                return videoProcessor.getNextFrame(frame);
            };

            displayManager.displayVideoInTerminal(frameProvider, selectedPalette.getSize(),
                                                  videoProcessor.getFPS());
        }
        else
        {
            // Display in new window
            std::cout << "Opening video in new window..." << std::endl;
            std::cout << "Dimensions: max " << maxWindowWidth << "x" << maxWindowHeight
                      << " (aspect ratio preserved)" << std::endl;
            std::cout << "Press Ctrl+C in the new window to stop playback." << std::endl;

            auto frameProvider = [&videoProcessor](cv::Mat &frame) -> bool
            {
                return videoProcessor.getNextFrame(frame);
            };

            displayManager.displayVideoInNewWindow(frameProvider, selectedPalette.getSize(),
                                                   videoProcessor.getFPS(),
                                                   maxWindowWidth, maxWindowHeight);
        }

        videoProcessor.release();
    }
    else
    {
        // Handle image display
        ImageProcessor imageProcessor;
        cv::Mat colorImage;

        // Load image based on color mode
        bool loaded = false;
        if (useColor) {
            loaded = imageProcessor.loadImageWithColor(mediaPath, colorImage);
        } else {
            loaded = imageProcessor.loadImage(mediaPath);
        }

        if (!loaded)
        {
            std::cerr << "Error: Could not load image file: " << mediaPath << std::endl;
            return -1;
        }

        // Process image: clamp pixel values to palette size
        cv::Mat processedImage = imageProcessor.clampPixelValues(0, selectedPalette.getSize() - 1);

        if (displayMode == 0)
        {
            // Display in current terminal
            if (useColor && !colorImage.empty()) {
                // Color mode - need to create a custom display method
                // For now, we'll update DisplayManager to handle this
                
                // Initialize ncurses with color
                initscr();
                noecho();
                curs_set(FALSE);
                
                if (has_colors()) {
                    start_color();
                    use_default_colors();
                    for (int i = 0; i < 16; i++) {
                        init_pair(i + 1, i, -1);
                    }
                }
                
                // Get terminal size and resize images
                int max_y, max_x;
                getmaxyx(stdscr, max_y, max_x);
                
                cv::Mat grayResized, colorResized;
                cv::resize(processedImage, grayResized, cv::Size(max_x, max_y));
                cv::resize(colorImage, colorResized, cv::Size(max_x, max_y));
                
                // Render with color
                int size = grayResized.rows * grayResized.cols;
                char* ascii_matrix = new char[size];
                int* color_pairs = new int[size];
                renderer.renderToMatrixWithColor(colorResized, grayResized, ascii_matrix, color_pairs);
                
                // Display
                clear();
                for (int i = 0; i < grayResized.rows; i++) {
                    for (int j = 0; j < grayResized.cols; j++) {
                        int idx = i * grayResized.cols + j;
                        attron(COLOR_PAIR(color_pairs[idx] + 1));
                        mvaddch(i, j, ascii_matrix[idx]);
                        attroff(COLOR_PAIR(color_pairs[idx] + 1));
                    }
                }
                refresh();
                
                // Wait for key press
                getch();
                
                // Cleanup
                delete[] ascii_matrix;
                delete[] color_pairs;
                endwin();
            } else {
                // Grayscale mode - use existing method
                displayManager.displayInTerminal(processedImage);
            }
        }
        else
        {
            // Display in new window
            std::cout << "Opening image in new window..." << std::endl;
            std::cout << "Dimensions: max " << maxWindowWidth << "x" << maxWindowHeight
                      << " (aspect ratio preserved)" << std::endl;
            if (useColor) {
                std::cout << "Note: External window mode uses grayscale only" << std::endl;
            }
            displayManager.displayImageInNewWindow(processedImage, maxWindowWidth, maxWindowHeight);
        }

        imageProcessor.release();
    }

    return 0;
}
