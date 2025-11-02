# OpenCV2Term - ASCII Art Image & Video Viewer

A terminal-based ASCII art viewer that converts images and videos to ASCII art using OpenCV and displays them interactively in the terminal.

## Features

- 🎨 **Multiple ASCII Palettes** - Choose from 3 carefully designed character sets
- 🖼️ **Image Support** - Display static images in ASCII art
- 🎬 **Video Support** - Play videos as ASCII art with proper frame timing
- 📺 **Dual Display Modes** - View in current terminal or open in new window
- 🔄 **Dynamic Resizing** - Automatically adapts to terminal size changes (images only)
- 📐 **Aspect Ratio Preservation** - Media maintains correct proportions
- ⚡ **Efficient Rendering** - Low CPU usage when idle
- 🧹 **Clean Resource Management** - Proper cleanup prevents memory leaks
- 🎯 **Static External Windows** - Clean, distraction-free display in separate window

## Project Structure

```
opencv2term/
├── include/                    # Header files
│   ├── AsciiPalette.h         # ASCII character palette management
│   ├── ImageProcessor.h       # Image loading and processing
│   ├── VideoProcessor.h       # Video loading and frame processing
│   ├── AsciiRenderer.h        # Media to ASCII conversion
│   ├── DisplayManager.h       # Display mode handling
│   └── MenuManager.h          # ncurses menu system
├── src/                       # Source files
│   ├── main.cpp               # Application entry point
│   ├── AsciiPalette.cpp       # Palette implementation
│   ├── ImageProcessor.cpp     # Image processing implementation
│   ├── VideoProcessor.cpp     # Video processing implementation
│   ├── AsciiRenderer.cpp      # Rendering implementation
│   ├── DisplayManager.cpp     # Display management
│   └── MenuManager.cpp        # Menu implementation
├── images/                    # Input media directory (gitignored)
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

### VideoProcessor
Handles video loading and frame extraction.
- Loads videos from files (.mp4, .avi, .mov, .mkv, etc.)
- Scans directories for video files
- Extracts frames sequentially
- Provides video metadata (FPS, dimensions, frame count)
- Converts frames to grayscale

### AsciiRenderer
Converts processed images to ASCII art.
- Converts images to ASCII character matrices
- Renders to string or character arrays
- Saves ASCII art to files

### DisplayManager
Manages display modes and terminal output.
- Interactive terminal display with resize handling (images)
- Video playback with proper frame timing
- New window display with calculated dimensions (static, clean)
- Signal handling for terminal resize events
- Cursor hiding and clean terminal state management

### MenuManager
Provides ncurses-based interactive menus.
- Palette selection menu with previews
- Media type selection (image or video)
- Image/video selection menu
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

1. **Add Media Files**: 
   - Images: .jpg, .png, .bmp, .gif, .tiff, etc.
   - Videos: .mp4, .avi, .mov, .mkv, .wmv, .flv, .webm
   - Place files in the `images/` directory

2. **Run the Program**:
   ```bash
   cd build
   ./OpenCVProject
   ```

3. **Navigate Menus**:
   - Use **UP/DOWN** arrow keys to navigate
   - Press **ENTER** to select
   - Press **Q** to quit

4. **Workflow**:
   - Select ASCII palette
   - Choose media type (Image or Video)
   - Select specific file
   - Choose display mode

5. **Images - Current Terminal** (Interactive):
   - Resize terminal window → image automatically adjusts
   - Press **ENTER** or **Q** to exit

6. **Images - New Window** (Static):
   - Opens in clean, separate Terminal window
   - Fixed dimensions, no extra output
   - Press any key to close

7. **Videos - Current Terminal** (Interactive):
   - Plays at correct FPS
   - Press **Q** or **ENTER** to stop

8. **Videos - New Window** (Static):
   - Opens in clean Terminal window
   - Loops continuously
   - Fixed dimensions, no extra output
   - Press **Ctrl+C** to close

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
4. **New Media Type**: Extend `VideoProcessor` or `ImageProcessor`

### Code Style
- Class-based architecture
- RAII for resource management
- Clear separation of concerns
- Minimal global state
- Functional callbacks for frame processing

## Video Support Details

### Implementation
- **Frame-by-frame processing**: Videos are processed one frame at a time
- **Accurate timing**: Maintains original FPS for smooth playback
- **Memory efficient**: Only one frame in memory at a time
- **Grayscale conversion**: All frames converted to grayscale automatically

### External Window Mode
- **Clean display**: No terminal prompts or extra text
- **Static dimensions**: Calculates size once, ignores resizes
- **Cursor hidden**: Clean viewing experience
- **Loop playback**: Videos loop continuously (exit with Ctrl+C)

### Supported Formats
All formats supported by OpenCV VideoCapture:
- MP4 (H.264, H.265)
- AVI
- MOV (QuickTime)
- MKV (Matroska)
- WMV (Windows Media)
- FLV (Flash Video)
- WebM

## License

This project is provided as-is for educational and personal use.

## Credits

- ASCII gradients based on research from jp2a and aalib
- OpenCV for image processing
- ncurses for terminal UI

