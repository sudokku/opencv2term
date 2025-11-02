#include <iostream>
#include <unistd.h>
#include "AsciiPalette.h"
#include "ImageProcessor.h"
#include "VideoProcessor.h"
#include "AsciiRenderer.h"
#include "DisplayManager.h"
#include "MenuManager.h"

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

    // Step 2: Select media type (image or video)
    int mediaType = menuManager.selectMediaType();
    if (mediaType < 0)
    {
        menuManager.cleanup();
        std::cout << "No media type selected. Exiting..." << std::endl;
        return 0;
    }

    std::string selectedMedia;
    bool isVideo = (mediaType == 1);

    // Step 3: Select specific media file
    if (isVideo)
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

    if (selectedMedia.empty())
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

    // Create renderer with selected palette
    AsciiRenderer renderer(&selectedPalette);
    DisplayManager displayManager(&renderer);

    if (isVideo)
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

        if (!imageProcessor.loadImage(mediaPath))
        {
            std::cerr << "Error: Could not load image file: " << mediaPath << std::endl;
            return -1;
        }

        // Process image: clamp pixel values to palette size
        cv::Mat processedImage = imageProcessor.clampPixelValues(0, selectedPalette.getSize() - 1);

        if (displayMode == 0)
        {
            // Display in current terminal
            displayManager.displayInTerminal(processedImage);
        }
        else
        {
            // Display in new window
            std::cout << "Opening image in new window..." << std::endl;
            std::cout << "Dimensions: max " << maxWindowWidth << "x" << maxWindowHeight
                      << " (aspect ratio preserved)" << std::endl;
            displayManager.displayImageInNewWindow(processedImage, maxWindowWidth, maxWindowHeight);
        }

        imageProcessor.release();
    }

    return 0;
}
