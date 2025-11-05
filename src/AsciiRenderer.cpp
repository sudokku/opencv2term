#include "AsciiRenderer.h"
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>

AsciiRenderer::AsciiRenderer(const AsciiPalette* palette, ColorMode colorMode)
    : palette_(palette), colorMode_(colorMode) {}

std::string AsciiRenderer::render(const cv::Mat& image) const {
    if (image.empty() || !palette_) return "";
    
    std::stringstream ss;
    
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            int pixelValue = image.at<uchar>(i, j);
            ss << palette_->getCharAt(pixelValue);
        }
        ss << '\n';
    }
    
    return ss.str();
}

void AsciiRenderer::renderToMatrix(const cv::Mat& image, char* outMatrix) const {
    if (image.empty() || !palette_ || !outMatrix) return;
    
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            int pixelValue = image.at<uchar>(i, j);
            outMatrix[i * image.cols + j] = palette_->getCharAt(pixelValue);
        }
    }
}

void AsciiRenderer::renderToMatrixWithColor(const cv::Mat& colorImage, const cv::Mat& grayImage,
                                            char* outMatrix, int* colorPairs) const {
    if (colorImage.empty() || grayImage.empty() || !palette_ || !outMatrix || !colorPairs) return;
    
    for (int i = 0; i < grayImage.rows; i++) {
        for (int j = 0; j < grayImage.cols; j++) {
            int idx = i * grayImage.cols + j;
            
            // Get brightness from grayscale image
            int pixelValue = grayImage.at<uchar>(i, j);
            outMatrix[idx] = palette_->getCharAt(pixelValue);
            
            // Get color from color image based on color mode
            if (colorMode_ != ColorMode::GRAYSCALE && colorImage.channels() == 3) {
                cv::Vec3b color = colorImage.at<cv::Vec3b>(i, j);
                // OpenCV uses BGR format
                int terminalColor;
                if (colorMode_ == ColorMode::COLOR_256) {
                    terminalColor = mapToTerminalColor256(color[2], color[1], color[0]);
                } else { // COLOR_16
                    terminalColor = mapToTerminalColor16(color[2], color[1], color[0]);
                }
                colorPairs[idx] = terminalColor;
            } else {
                colorPairs[idx] = 7;  // Default white
            }
        }
    }
}

int AsciiRenderer::mapToTerminalColor16(int r, int g, int b) {
    // Map RGB (0-255) to 16 basic terminal colors
    // Colors 0-7: Black, Red, Green, Yellow, Blue, Magenta, Cyan, White
    // Colors 8-15: Bright versions of above
    
    // Calculate brightness (weighted for human perception)
    int brightness = (r * 299 + g * 587 + b * 114) / 1000;
    
    // If very dark, return black
    if (brightness < 20) return 0;
    
    // Balanced threshold - 64 (25% of 255)
    const int threshold = 64;
    
    // Check which color channels are active
    int rNorm = r > threshold ? 1 : 0;
    int gNorm = g > threshold ? 1 : 0;
    int bNorm = b > threshold ? 1 : 0;
    
    // Consider color dominance for more vibrant colors
    // If one channel is significantly stronger, emphasize it
    int maxChannel = std::max({r, g, b});
    int minChannel = std::min({r, g, b});
    int colorDifference = maxChannel - minChannel;
    
    // If there's significant color variation (not grayscale)
    if (colorDifference > 50) {
        // Find dominant color(s)
        // Use threshold relative to max channel
        const int dominanceThreshold = maxChannel - 30;
        rNorm = (r > dominanceThreshold) ? 1 : 0;
        gNorm = (g > dominanceThreshold) ? 1 : 0;
        bNorm = (b > dominanceThreshold) ? 1 : 0;
    }
    
    // Determine base color (0-7)
    int baseColor = (rNorm * 1) + (gNorm * 2) + (bNorm * 4);
    
    // If all channels are similar and bright, use white
    if (brightness > 180 && colorDifference < 40) {
        return 15; // Bright white
    }
    
    // Use bright version if brightness is above threshold
    bool isBright = brightness > 100;
    return isBright ? (baseColor + 8) : baseColor;
}

int AsciiRenderer::mapToTerminalColor256(int r, int g, int b) {
    // Map RGB (0-255) to 256 terminal colors
    // 256-color palette:
    //   0-15:   Basic 16 colors (same as 16-color mode)
    //   16-231: 6x6x6 RGB cube (216 colors)
    //   232-255: Grayscale ramp (24 shades)
    
    // Calculate brightness
    int brightness = (r * 299 + g * 587 + b * 114) / 1000;
    
    // If very dark, use black
    if (brightness < 8) return 0;
    
    // Check if it's grayscale (all channels similar)
    int maxChannel = std::max({r, g, b});
    int minChannel = std::min({r, g, b});
    int colorDifference = maxChannel - minChannel;
    
    // If it's grayscale (channels differ by less than 10), use grayscale ramp (232-255)
    if (colorDifference < 10) {
        // Map brightness 0-255 to grayscale colors 232-255 (24 shades)
        // But keep pure black as 0 and pure white as 15
        if (brightness < 8) return 0;
        if (brightness > 247) return 15;
        return 232 + (brightness * 23 / 255);
    }
    
    // Use the 6x6x6 RGB cube (colors 16-231)
    // Each channel is divided into 6 levels (0-5)
    // Formula: 16 + 36*r + 6*g + b
    
    // Map 0-255 to 0-5 for each channel
    // Using 51 as the divisor (255/5 = 51)
    int r6 = (r * 5 + 127) / 255;  // Round to nearest
    int g6 = (g * 5 + 127) / 255;
    int b6 = (b * 5 + 127) / 255;
    
    // Clamp to 0-5
    r6 = std::min(5, std::max(0, r6));
    g6 = std::min(5, std::max(0, g6));
    b6 = std::min(5, std::max(0, b6));
    
    return 16 + (36 * r6) + (6 * g6) + b6;
}

bool AsciiRenderer::saveToFile(const cv::Mat& image, const std::string& filename) const {
    if (image.empty() || !palette_) return false;
    
    std::ofstream outFile(filename);
    if (!outFile.is_open()) return false;
    
    outFile << render(image);
    outFile.close();
    
    return true;
}

