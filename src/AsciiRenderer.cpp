#include "AsciiRenderer.h"
#include <fstream>
#include <sstream>
#include <cmath>

AsciiRenderer::AsciiRenderer(const AsciiPalette* palette, bool useColor)
    : palette_(palette), useColor_(useColor) {}

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
            
            // Get color from color image
            if (useColor_ && colorImage.channels() == 3) {
                cv::Vec3b color = colorImage.at<cv::Vec3b>(i, j);
                // OpenCV uses BGR format
                int terminalColor = mapToTerminalColor(color[2], color[1], color[0]);
                colorPairs[idx] = terminalColor;
            } else {
                colorPairs[idx] = 7;  // Default white
            }
        }
    }
}

int AsciiRenderer::mapToTerminalColor(int r, int g, int b) {
    // Map RGB (0-255) to 16 basic terminal colors
    // Colors 0-7: Black, Red, Green, Yellow, Blue, Magenta, Cyan, White
    // Colors 8-15: Bright versions of above
    
    // Calculate brightness
    int brightness = (r + g + b) / 3;
    bool isBright = brightness > 127;
    
    // Normalize colors
    int rNorm = r > 85 ? 1 : 0;
    int gNorm = g > 85 ? 1 : 0;
    int bNorm = b > 85 ? 1 : 0;
    
    // Determine base color (0-7)
    int baseColor = (rNorm * 1) + (gNorm * 2) + (bNorm * 4);
    
    // If very dark, return black
    if (brightness < 30) return 0;
    
    // If very bright and mostly white, return bright white
    if (brightness > 200 && rNorm && gNorm && bNorm) return 15;
    
    // Return bright version if bright enough
    return isBright ? (baseColor + 8) : baseColor;
}

bool AsciiRenderer::saveToFile(const cv::Mat& image, const std::string& filename) const {
    if (image.empty() || !palette_) return false;
    
    std::ofstream outFile(filename);
    if (!outFile.is_open()) return false;
    
    outFile << render(image);
    outFile.close();
    
    return true;
}

