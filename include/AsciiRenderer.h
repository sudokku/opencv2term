#ifndef ASCII_RENDERER_H
#define ASCII_RENDERER_H

#include <opencv2/opencv.hpp>
#include <string>
#include "AsciiPalette.h"

class AsciiRenderer {
public:
    AsciiRenderer(const AsciiPalette* palette);
    
    // Convert image to ASCII characters
    std::string render(const cv::Mat& image) const;
    
    // Render to a character matrix (for ncurses)
    void renderToMatrix(const cv::Mat& image, char* outMatrix) const;
    
    // Save ASCII art to file
    bool saveToFile(const cv::Mat& image, const std::string& filename) const;
    
    void setPalette(const AsciiPalette* palette) { palette_ = palette; }
    
private:
    const AsciiPalette* palette_;
};

#endif // ASCII_RENDERER_H

