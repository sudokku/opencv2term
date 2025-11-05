#ifndef ASCII_RENDERER_H
#define ASCII_RENDERER_H

#include <opencv2/opencv.hpp>
#include <string>
#include "AsciiPalette.h"

enum class ColorMode {
    GRAYSCALE,
    COLOR_16,
    COLOR_256
};

class AsciiRenderer {
public:
    AsciiRenderer(const AsciiPalette* palette, ColorMode colorMode = ColorMode::GRAYSCALE);
    
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
    void setColorMode(ColorMode colorMode) { colorMode_ = colorMode; }
    ColorMode getColorMode() const { return colorMode_; }
    bool isUsingColor() const { return colorMode_ != ColorMode::GRAYSCALE; }
    
    // Map RGB color to nearest terminal color (0-15 for 16-color mode)
    static int mapToTerminalColor16(int r, int g, int b);
    
    // Map RGB color to nearest terminal color (0-255 for 256-color mode)
    static int mapToTerminalColor256(int r, int g, int b);
    
private:
    const AsciiPalette* palette_;
    ColorMode colorMode_;
};

#endif // ASCII_RENDERER_H

