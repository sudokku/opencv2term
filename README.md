# OpenCV2Term - ASCII Art Image, Video & Live Camera Viewer

A terminal-based ASCII art viewer that converts images, videos, and live camera feeds to ASCII art using OpenCV and displays them interactively in the terminal.

## Features

- 🎨 **Multiple ASCII Palettes** - Choose from 3 carefully designed character sets
- 🌈 **Color Support** - 16-color terminal display with RGB mapping
- 🖼️ **Image Support** - Display static images in ASCII art
- 🎬 **Video Support** - Play videos as ASCII art with proper frame timing
- 📹 **Live Camera** - Real-time ASCII art from your webcam
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
│   ├── CameraProcessor.h      # Live camera feed processing
│   ├── AsciiRenderer.h        # Media to ASCII conversion
│   ├── DisplayManager.h       # Display mode handling
│   └── MenuManager.h          # ncurses menu system
├── src/                       # Source files
│   ├── main.cpp               # Application entry point
│   ├── AsciiPalette.cpp       # Palette implementation
│   ├── ImageProcessor.cpp     # Image processing implementation
│   ├── VideoProcessor.cpp     # Video processing implementation
│   ├── CameraProcessor.cpp    # Camera processing implementation
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

### CameraProcessor
Handles live camera feed capture.
- Opens camera device (default or by ID)
- Captures frames in real-time from webcam
- Provides camera metadata (FPS, resolution)
- Converts frames to grayscale
- Efficient frame capture with minimal latency

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
- Media type selection (image, video, or camera)
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
   - Camera: Built-in webcam (no files needed)
   - Place image/video files in the `images/` directory

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
   - Choose color mode (Grayscale or Color)
   - Choose media type (Image, Video, or Live Camera)
   - Select specific file (if Image/Video)
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

9. **Live Camera - Current Terminal** (Interactive):
   - Real-time feed from your webcam
   - Press **Q** or **ENTER** to stop
   - Note: Camera only works in current terminal mode

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

## Color Support

### Modes

**Grayscale (Classic)**
- Traditional monochrome ASCII art
- Uses brightness values only
- Fastest performance
- Works on all terminals

**Color (16-Color)**
- Colorized ASCII art using terminal colors
- Maps RGB values to 16 ANSI colors
- More vibrant and recognizable
- Requires color-capable terminal

### How It Works

The color system:
1. Analyzes each pixel's RGB values
2. Maps to nearest of 16 terminal colors:
   - 0-7: Black, Red, Green, Yellow, Blue, Magenta, Cyan, White
   - 8-15: Bright versions of above
3. Applies color to ASCII character while preserving brightness-based character selection

### Color Mapping

```
Pixel Color          →  Terminal Color
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Bright Red (255,0,0)  →  9 (Bright Red)
Sky Blue (0,128,255)  →  14 (Bright Cyan)
Green (0,255,0)       →  10 (Bright Green)
Purple (128,0,128)    →  5 (Magenta)
Orange (255,128,0)    →  11 (Bright Yellow)
Dark Gray (64,64,64)  →  0 (Black)
White (255,255,255)   →  15 (Bright White)
```

### When to Use Color

**Use Color For:**
- Portraits and faces (skin tones)
- Nature scenes (green/blue)
- Logos and graphics
- Art and illustrations
- Camera/video feeds (more recognizable)

**Use Grayscale For:**
- Text documents
- High-contrast images
- Performance-critical applications (Raspberry Pi)
- Monochrome aesthetic
- Maximum compatibility

### Performance Impact

- **CPU**: ~5-10% overhead compared to grayscale
- **FPS**: Minimal impact (1-2 FPS reduction)
- **Raspberry Pi**: Still achieves 25-28 FPS with color on Pi 5

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

## Camera Support Details

### Implementation
- **Device detection**: Uses default camera (device ID 0)
- **Real-time capture**: Frame-by-frame from webcam
- **Automatic conversion**: Grayscale processing pipeline
- **FPS detection**: Automatically detects camera FPS (defaults to 30 if unavailable)

### Requirements
- **Camera access**: Webcam must be connected and available
- **Permissions**: Terminal/app must have camera permissions
- **No conflicts**: No other app should be using the camera

### Troubleshooting
If camera fails to open:
1. Check camera is physically connected
2. Verify no other app (Zoom, Teams, etc.) is using the camera
3. Grant camera permissions:
   - macOS: System Settings → Privacy & Security → Camera
   - Check "Terminal" has camera access enabled
4. Try closing other applications and retrying

### Limitations
- Camera display only works in **current terminal mode**
- External window mode falls back to current terminal
- This is to maintain live, real-time rendering

### Performance
- **Latency**: Minimal (dependent on camera FPS)
- **CPU Usage**: Moderate during active capture
- **Resolution**: Automatically scaled to terminal size

## License

This project is provided as-is for educational and personal use.

## Credits

- ASCII gradients based on research from jp2a and aalib
- OpenCV for image processing
- ncurses for terminal UI

