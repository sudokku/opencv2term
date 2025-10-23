# OpenCV2Term - ASCII Art Image Viewer

A terminal-based ASCII art viewer that converts images to ASCII art using OpenCV and displays them interactively in the terminal.

## Features

- 🎨 **Multiple ASCII Palettes** - Choose from 3 carefully designed character sets
- 🖼️ **Interactive Image Selection** - Browse and select images from a directory
- 📺 **Dual Display Modes** - View in current terminal or open in new window
- 🔄 **Dynamic Resizing** - Automatically adapts to terminal size changes
- 📐 **Aspect Ratio Preservation** - Images maintain correct proportions
- ⚡ **Efficient Rendering** - Low CPU usage when idle
- 🧹 **Clean Resource Management** - Proper cleanup prevents memory leaks

## Project Structure

```
opencv2term/
├── include/                    # Header files
│   ├── AsciiPalette.h         # ASCII character palette management
│   ├── ImageProcessor.h       # Image loading and processing
│   ├── AsciiRenderer.h        # Image to ASCII conversion
│   ├── DisplayManager.h       # Display mode handling
│   └── MenuManager.h          # ncurses menu system
├── src/                       # Source files
│   ├── main.cpp               # Application entry point
│   ├── AsciiPalette.cpp       # Palette implementation
│   ├── ImageProcessor.cpp     # Image processing implementation
│   ├── AsciiRenderer.cpp      # Rendering implementation
│   ├── DisplayManager.cpp     # Display management
│   └── MenuManager.cpp        # Menu implementation
├── images/                    # Input images directory (gitignored)
├── build/                     # Build artifacts (gitignored)
├── CMakeLists.txt            # CMake configuration
└── README.md                 # This file

```

## Class Architecture

### AsciiPalette
Manages ASCII character palettes for rendering.
- Stores palette name and character gradient
- Provides default palette presets
- Maps pixel values to characters

### ImageProcessor
Handles image loading and preprocessing.
- Loads images from files
- Scans directories for image files
- Clamps pixel values to palette range
- Resizes images with aspect ratio preservation

### AsciiRenderer
Converts processed images to ASCII art.
- Converts images to ASCII character matrices
- Renders to string or character arrays
- Saves ASCII art to files

### DisplayManager
Manages display modes and terminal output.
- Interactive terminal display with resize handling
- New window display with calculated dimensions
- Signal handling for terminal resize events

### MenuManager
Provides ncurses-based interactive menus.
- Palette selection menu with previews
- Image selection menu
- Display mode selection menu
- Generic menu implementation with arrow key navigation

## Building

### Prerequisites

- CMake 3.10+
- C++17 compiler
- OpenCV 4.x
- ncurses library

### macOS
```bash
brew install cmake opencv
```

### Build Instructions

```bash
# Create build directory
mkdir -p build
cd build

# Configure
cmake ..

# Compile
make

# Run
./OpenCVProject
```

## Usage

1. **Add Images**: Place image files (.jpg, .png, .bmp, etc.) in the `images/` directory

2. **Run the Program**:
   ```bash
   cd build
   ./OpenCVProject
   ```

3. **Navigate Menus**:
   - Use **UP/DOWN** arrow keys to navigate
   - Press **ENTER** to select
   - Press **Q** to quit

4. **Interactive Display** (Mode 1):
   - Resize terminal window → image automatically adjusts
   - Press **ENTER** to exit

5. **New Window Display** (Mode 2):
   - Opens in separate Terminal window
   - Fixed dimensions with proper aspect ratio
   - Press any key in new window to close

## ASCII Palettes

### Standard (10 levels)
` .:-=+*#%@`
- Simple, fast rendering
- Good for quick previews
- Works on all terminals

### Balanced (20 levels)
` .':,;!/>+=*oahkO0Q#MW@`
- Best general-purpose option
- Good detail without complexity
- Recommended for most images

### Detailed (70 levels)
` .'`^",:;Il!i><~+_-?][}{1)(|\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$`
- Maximum detail and shading
- Used by professional tools
- Best for high-quality output

## Performance

- **Idle CPU Usage**: ~0% (efficient event-driven architecture)
- **Memory Management**: Automatic cleanup prevents leaks
- **Signal Handling**: Safe resize handling with proper cleanup

## Development

### Adding New Features

1. **New Palette**: Add to `AsciiPalette::getDefaultPalettes()`
2. **New Display Mode**: Extend `DisplayManager` class
3. **New Menu**: Add method to `MenuManager` class

### Code Style
- Class-based architecture
- RAII for resource management
- Clear separation of concerns
- Minimal global state

## License

This project is provided as-is for educational and personal use.

## Credits

- ASCII gradients based on research from jp2a and aalib
- OpenCV for image processing
- ncurses for terminal UI

