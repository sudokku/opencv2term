#include "AsciiRenderer.h"
#include <fstream>
#include <sstream>

AsciiRenderer::AsciiRenderer(const AsciiPalette* palette)
    : palette_(palette) {}

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

bool AsciiRenderer::saveToFile(const cv::Mat& image, const std::string& filename) const {
    if (image.empty() || !palette_) return false;
    
    std::ofstream outFile(filename);
    if (!outFile.is_open()) return false;
    
    outFile << render(image);
    outFile.close();
    
    return true;
}

