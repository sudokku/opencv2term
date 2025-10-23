#include "AsciiPalette.h"

AsciiPalette::AsciiPalette(const std::string& name, const std::string& characters)
    : name_(name), characters_(characters), size_(characters.length()) {}

char AsciiPalette::getCharAt(int index) const {
    if (index >= 0 && index < size_) {
        return characters_[index];
    }
    return ' '; // Default to space for out of bounds
}

std::vector<AsciiPalette> AsciiPalette::getDefaultPalettes() {
    return {
        AsciiPalette("Standard (10 levels)", " .:-=+*#%@"),
        AsciiPalette("Detailed (70 levels)", " .'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$"),
        AsciiPalette("Balanced (20 levels)", " .':,;!/>+=*oahkO0Q#MW@")
    };
}

