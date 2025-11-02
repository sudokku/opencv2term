#ifndef ASCII_RENDERER_H
#define ASCII_RENDERER_H

#include <opencv2/opencv.hpp>
#include <string>
#include "AsciiPalette.h"

class AsciiRenderer {
public:
    AsciiRenderer(const AsciiPalette* palette, bool useColor = false);
    
    // Convert image to ASCII characters
    std::string render(const cv::Mat& image) const;
    
    // Render to a character matrix (for ncurses)
    void renderToMatrix(const cv::Mat& image, char* outMatrix) const;
    
    // Render to character matrix with color information (for ncurses with color)
    void renderToMatrixWithColor(const cv::Mat& colorImage, const cv::Mat& grayImage, 
                                 char* outMatrix, int* colorPairs) const;
    
    // Save ASCII art to file
    bool saveToFile(const cv::Mat& image, const std::string& filename) const;
    
    void setPalette(const AsciiPalette* palette) { palette_ = palette; }
    void setUseColor(bool useColor) { useColor_ = useColor; }
    bool isUsingColor() const { return useColor_; }
    
    // Map RGB color to nearest terminal color (0-15)
    static int mapToTerminalColor(int r, int g, int b);
    
private:
    const AsciiPalette* palette_;
    bool useColor_;
};

#endif // ASCII_RENDERER_H

