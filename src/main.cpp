#include <iostream>
#include "AsciiPalette.h"
#include "ImageProcessor.h"
#include "AsciiRenderer.h"
#include "DisplayManager.h"
#include "MenuManager.h"

int main(int argc, char** argv) {
    // Configuration
    const std::string imagesDir = "../images";
    const int maxWindowWidth = 120;
    const int maxWindowHeight = 60;
    
    // Get available images
    auto imageFiles = ImageProcessor::getImageFiles(imagesDir);
    if (imageFiles.empty()) {
        std::cerr << "No image files found in " << imagesDir << " directory" << std::endl;
        std::cerr << "Please add some image files (.jpg, .png, .bmp, etc.) to the images directory" << std::endl;
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
    
    // Step 2: Select image
    std::string selectedImage = menuManager.selectImage(imageFiles);
    if (selectedImage.empty()) {
        menuManager.cleanup();
        std::cout << "No image selected. Exiting..." << std::endl;
        return 0;
    }
    
    // Step 3: Select display mode
    int displayMode = menuManager.selectDisplayMode();
    if (displayMode < 0) {
        menuManager.cleanup();
        std::cout << "No display mode selected. Exiting..." << std::endl;
        return 0;
    }
    
    // Clean up menu UI
    menuManager.cleanup();
    
    // Load the image
    ImageProcessor imageProcessor;
    std::string imagePath = imagesDir + "/" + selectedImage;
    
    if (!imageProcessor.loadImage(imagePath)) {
        std::cerr << "Error: Could not load image file: " << imagePath << std::endl;
        return -1;
    }
    
    // Process image: clamp pixel values to palette size
    cv::Mat processedImage = imageProcessor.clampPixelValues(0, selectedPalette.getSize() - 1);
    
    // Create renderer with selected palette
    AsciiRenderer renderer(&selectedPalette);
    
    // Display based on selected mode
    DisplayManager displayManager(&renderer);
    
    if (displayMode == 0) {
        // Mode 0: Display in current terminal
        displayManager.displayInTerminal(processedImage);
    } else if (displayMode == 1) {
        // Mode 1: Open in new window
        std::cout << "Opening ASCII art in new window..." << std::endl;
        std::cout << "Dimensions: max " << maxWindowWidth << "x" << maxWindowHeight 
                  << " (aspect ratio preserved)" << std::endl;
        displayManager.displayInNewWindow(processedImage, maxWindowWidth, maxWindowHeight);
    }
    
    // Cleanup
    imageProcessor.release();
    
    return 0;
}

