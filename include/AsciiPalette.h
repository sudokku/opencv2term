#ifndef ASCII_PALETTE_H
#define ASCII_PALETTE_H

#include <string>
#include <vector>

class AsciiPalette {
public:
    AsciiPalette(const std::string& name, const std::string& characters);
    
    const std::string& getName() const { return name_; }
    const std::string& getCharacters() const { return characters_; }
    int getSize() const { return size_; }
    char getCharAt(int index) const;
    
    // Static method to get all available palettes
    static std::vector<AsciiPalette> getDefaultPalettes();
    
private:
    std::string name_;
    std::string characters_;
    int size_;
};

#endif // ASCII_PALETTE_H

