# opencv2term

A terminal ASCII art renderer that converts images, videos, and live camera feeds to ASCII art using OpenCV and ncurses.

## Features

- Three color modes: GRAYSCALE, COLOR_16, COLOR_256
- Three ASCII palettes: Standard (10), Balanced (20), Detailed (70)
- Three input types: static images, video files, live camera
- Three image modes: current terminal, new terminal window, or generated PNG
- Frame-accurate video playback at original FPS
- Terminal resize handling via SIGWINCH (images in current terminal mode)
- Aspect ratio preservation for all media types
- Interactive ncurses menu with arrow key navigation
- Generated ASCII image export at 64, 128, or 256 characters on the shorter edge

## Project Structure

```
opencv2term/
├── include/
│   ├── AsciiPalette.h
│   ├── AsciiRenderer.h
│   ├── DisplayManager.h
│   ├── MenuManager.h
│   ├── ImageProcessor.h
│   ├── VideoProcessor.h
│   └── CameraProcessor.h
├── src/
│   ├── main.cpp
│   ├── AsciiPalette.cpp
│   ├── AsciiRenderer.cpp
│   ├── DisplayManager.cpp
│   ├── MenuManager.cpp
│   ├── ImageProcessor.cpp
│   ├── VideoProcessor.cpp
│   └── CameraProcessor.cpp
├── images/              # Input media directory (gitignored)
├── outputs/             # Generated ASCII images (gitignored)
├── build/               # Build artifacts (gitignored)
├── CMakeLists.txt
└── README.md
```

## Building

### Prerequisites

- CMake 3.10+
- C++17 compiler
- OpenCV 4.x
- ncurses

### macOS

```bash
brew install cmake opencv
```

### Build

```bash
mkdir -p build && cd build
cmake ..
make
```

### Run

```bash
cd build
./OpenCVProject
```

Media files must be placed in `images/` relative to the project root (one level above the binary).

## Usage

1. Place image or video files in the `images/` directory.
2. Run `./OpenCVProject` from the `build/` directory.
3. Follow the interactive menu:
   - Select an ASCII palette
   - Select a color mode
   - Select media type (Image, Video, or Camera)
   - Select a file (Image and Video only)
   - Select a display mode
   - Select an output density if generating an ASCII image file

### Menu Navigation

- UP/DOWN arrow keys to move
- ENTER to select
- Q to quit

### Display Behavior

| Mode | Input | Behavior |
|------|-------|----------|
| Current terminal | Image | Live resize on SIGWINCH, press Q or ENTER to exit |
| Current terminal | Video | Plays at original FPS, press Q or ENTER to stop |
| Current terminal | Camera | Real-time webcam feed, press Q or ENTER to stop |
| New window | Image | Opens separate terminal window, press any key to close |
| New window | Video | Opens separate terminal window, loops until Ctrl+C |
| New window | Camera | Falls back to current terminal mode |
| Generated PNG | Image | Writes a dark-background ASCII image to `outputs/` |

### Generated Images

Image inputs can be exported as PNG files with dark backgrounds. The size presets control ASCII density, not final bitmap dimensions:

| Density | ASCII cells |
|---------|-------------|
| Small | 64 characters on the shorter edge |
| Medium | 128 characters on the shorter edge |
| Large | 256 characters on the shorter edge |

The longer edge is calculated from the source image aspect ratio and measured character-cell dimensions. Rendering accounts for glyph width, baseline, and line height before resizing the source into ASCII cells, so the result is not stretched as if every ASCII character were a 1x1 pixel.

The final PNG pixel dimensions depend on the input image size and selected density. The renderer does not add extra letter spacing or line spacing, which keeps the ASCII texture tight and readable.

## ASCII Palettes

| Name | Characters | Description |
|------|-----------|-------------|
| Standard | 10 | `. : - = + * # % @` — fast, minimal |
| Balanced | 20 | Extended set with punctuation — good general purpose |
| Detailed | 70 | Full gradient — maximum shading and detail |

## Color Modes

**GRAYSCALE**
- Monochrome output using brightness-mapped characters only.
- Fastest rendering, works on all terminals.

**COLOR_16**
- Maps each pixel's RGB values to the nearest of 16 ANSI terminal colors (colors 0-15).
- Requires a color-capable terminal.

**COLOR_256**
- Maps pixels to a 216-color RGB cube (colors 16-231) for finer color fidelity.
- Requires a 256-color terminal (most modern terminals qualify).
- Uses up to 256 ncurses color pairs.

All three modes use the same brightness-based character selection; color mode only affects the foreground color applied to each character.

## Supported Formats

### Images
`.jpg`, `.jpeg`, `.png`, `.webp`, `.bmp`, `.gif`, `.tiff`, `.tif`

### Video
`.mp4`, `.avi`, `.mov`, `.mkv`, `.wmv`, `.flv`, `.webm`

All formats are handled by OpenCV; actual support depends on the codecs available in your OpenCV build.

### Camera
Default capture device (ID 0). Camera access requires appropriate OS permissions.

**macOS**: System Settings → Privacy & Security → Camera → enable Terminal.

## Development

### Extending the Project

- **New palette**: add to `AsciiPalette::getDefaultPalettes()` in `src/AsciiPalette.cpp`
- **New color mode**: extend `AsciiRenderer::renderToMatrixWithColor()` and add initialization in `DisplayManager::initializeColors()`
- **New display mode**: add a method to `DisplayManager`
- **New media type**: create a new processor class following the `VideoProcessor` / `CameraProcessor` pattern and wire it into `src/main.cpp`

### Code Conventions

- Class-based architecture with clear separation of concerns
- RAII for resource management (ncurses `endwin()`, OpenCV `cap.release()`)
- Functional callbacks (`frameProvider`) passed to `DisplayManager::displayVideoInTerminal()`
- Minimal global state

## Credits

- ASCII gradient research: jp2a, aalib
- Image processing: OpenCV
- Terminal UI: ncurses
