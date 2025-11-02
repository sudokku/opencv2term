# Raspberry Pi Digital Mirror Setup Guide

Complete step-by-step guide to deploy your ASCII Art Digital Mirror on Raspberry Pi.

## Hardware Requirements

### Required Components
- **Raspberry Pi 5 (8GB)** or **Pi 4 (8GB)** - $75-80
- **Pi Camera Module 3** or **USB Webcam** - $20-25
- **Official Pi Power Supply** (27W for Pi 5, 15W for Pi 4) - $12
- **MicroSD Card 64GB** (A2 Class, high speed) - $10
- **Micro HDMI to HDMI Cable** - $8
- **Pi Case with Fan/Heatsinks** - $15
- **Full HD TV 32"-43"** (secondhand) - $50-150

### Optional Components
- Wall mount bracket - $15-30
- Frame for mirror aesthetic - $20-50
- USB extension cable for camera - $5-10
- LED backlight strip - $10-20

**Total Cost: $225-300**

---

## Part 1: Initial Setup

### Step 1: Prepare SD Card

1. **Download Raspberry Pi Imager**
   - Visit: https://www.raspberrypi.com/software/
   - Install on your computer

2. **Flash Operating System**
   ```
   - Open Raspberry Pi Imager
   - Choose Device: Raspberry Pi 5 (or your model)
   - Choose OS: Raspberry Pi OS (64-bit) Lite
   - Choose Storage: Your SD card
   - Click "Next"
   ```

3. **Configure Settings** (click "Edit Settings")
   ```
   General:
   - Set hostname: ascii-mirror
   - Enable SSH
   - Set username: pi
   - Set password: [your password]
   - Configure WiFi (SSID and password)
   
   Services:
   - Enable SSH (Use password authentication)
   ```

4. **Write to SD Card**
   - Click "Yes" to apply settings
   - Click "Yes" to confirm erase
   - Wait for write and verification

### Step 2: First Boot

1. **Insert SD Card** into Raspberry Pi
2. **Connect Hardware**:
   - HDMI cable to TV
   - Power supply
   - Camera (if using Pi Camera Module)
   - Keyboard (for initial setup)
3. **Power On**
4. **Wait for Boot** (~30-60 seconds)

### Step 3: Initial Configuration

```bash
# Update system
sudo apt update
sudo apt upgrade -y

# Install essential tools
sudo apt install -y vim git htop

# Enable camera (if using Pi Camera Module)
sudo raspi-config
# Navigate to: Interface Options → Camera → Enable
# Reboot when prompted

# For Pi 5: Enable legacy camera support if needed
sudo raspi-config
# Navigate to: Interface Options → Legacy Camera → Enable
```

---

## Part 2: Install Dependencies

### OpenCV Installation

```bash
# Install OpenCV and dependencies
sudo apt install -y libopencv-dev python3-opencv

# Install additional required libraries
sudo apt install -y \
    build-essential \
    cmake \
    pkg-config \
    libjpeg-dev \
    libpng-dev \
    libtiff-dev \
    libavcodec-dev \
    libavformat-dev \
    libswscale-dev \
    libv4l-dev \
    libxvidcore-dev \
    libx264-dev

# Install ncurses
sudo apt install -y libncurses5-dev libncursesw5-dev
```

### Verify Installation

```bash
# Check OpenCV version
pkg-config --modversion opencv4

# Should show: 4.5.x or higher

# Test camera
v4l2-ctl --list-devices

# Should list your camera device
```

---

## Part 3: Deploy Your Project

### Clone or Transfer Project

**Option A: From GitHub (if you've pushed it)**
```bash
cd ~
git clone https://github.com/yourusername/opencv2term.git
cd opencv2term
```

**Option B: Transfer from your Mac**
```bash
# On your Mac (from project root):
rsync -avz --exclude 'build' --exclude '.git' \
  ~/Developer/opencv2term/ pi@ascii-mirror.local:~/opencv2term/

# Then SSH to Pi:
ssh pi@ascii-mirror.local
cd ~/opencv2term
```

### Build with Optimizations

```bash
cd ~/opencv2term

# Create optimized build directory
mkdir -p build
cd build

# Configure with optimizations
cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_FLAGS="-O3 -march=native" \
  ..

# Build (use all cores)
make -j$(nproc)

# Verify build
ls -lh OpenCVProject
# Should show executable
```

### Test Run

```bash
# Run the program
./OpenCVProject

# Test with camera:
# 1. Select palette (Standard recommended for Pi)
# 2. Choose "Live Camera"
# 3. Select "Current terminal"
# Press Q to exit

# Check FPS and performance
```

---

## Part 4: Performance Optimization

### Optimize Camera Settings

Edit `src/CameraProcessor.cpp` to add after line 12:

```cpp
bool CameraProcessor::openCamera(int deviceId) {
    capture_.open(deviceId);
    
    if (!capture_.isOpened()) {
        return false;
    }
    
    // Optimize for Raspberry Pi
    capture_.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    capture_.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    capture_.set(cv::CAP_PROP_FPS, 30);
    capture_.set(cv::CAP_PROP_BUFFERSIZE, 1);  // Minimize latency
    
    // ... rest of function
}
```

### Enable GPU Acceleration

```bash
# Edit boot config
sudo nano /boot/firmware/config.txt

# Add these lines:
dtoverlay=vc4-kms-v3d
gpu_mem=256

# Save and reboot
sudo reboot
```

### Reduce Terminal Size

For better performance, use smaller terminal grid:

```cpp
// In DisplayManager.cpp, modify maxWidth/maxHeight
const int maxWindowWidth = 100;   // Instead of 120
const int maxWindowHeight = 50;   // Instead of 60
```

---

## Part 5: Auto-Start Configuration

### Create Systemd Service

```bash
# Create service file
sudo nano /etc/systemd/system/ascii-mirror.service
```

**Add this content:**

```ini
[Unit]
Description=ASCII Art Digital Mirror
After=graphical.target network.target
Wants=graphical.target

[Service]
Type=simple
User=pi
WorkingDirectory=/home/pi/opencv2term/build
Environment="TERM=xterm-256color"
Environment="DISPLAY=:0"
StandardInput=tty
StandardOutput=tty
TTYPath=/dev/tty1
TTYReset=yes
TTYVHangup=yes

# Pre-select options (auto-start with Standard palette, Camera, Current terminal)
# You'll need to modify main.cpp to support command-line args for this

ExecStart=/home/pi/opencv2term/build/OpenCVProject
Restart=always
RestartSec=3

[Install]
WantedBy=multi-user.target
```

**Enable and start:**

```bash
# Reload systemd
sudo systemctl daemon-reload

# Enable service
sudo systemctl enable ascii-mirror.service

# Start service
sudo systemctl start ascii-mirror.service

# Check status
sudo systemctl status ascii-mirror.service

# View logs
journalctl -u ascii-mirror.service -f
```

### Disable Screen Blanking

```bash
# Edit boot config
sudo nano /boot/firmware/config.txt

# Add:
hdmi_blanking=1

# Disable screen saver
sudo nano /etc/xdg/lxsession/LXDE-pi/autostart

# Add:
@xset s off
@xset -dpms
@xset s noblank
```

---

## Part 6: Physical Installation

### 1. Camera Positioning

**For Mirror Effect:**
- Mount camera at top center of TV
- Angle slightly downward (5-10°)
- Distance: 2-3 feet from expected viewer position
- Use USB extension if needed for positioning

**Camera Orientation Test:**
```bash
# Run program and test camera position
cd ~/opencv2term/build
./OpenCVProject

# Adjust camera until framing is good
```

### 2. Pi Mounting

**Behind TV:**
- Use VESA mount adapter or adhesive mount
- Ensure ventilation (Pi will get warm)
- Keep away from TV heat vents
- Organize cables neatly

**Cooling:**
- Use case with fan (active cooling recommended)
- Ensure air flow
- Monitor temperature: `vcgencmd measure_temp`

### 3. Cable Management

```
TV (back):
├─ Power cable
├─ HDMI from Pi
├─ USB cable to camera (if external)
└─ Pi power cable

Keep cables:
- Organized with zip ties
- Away from heat sources
- Accessible for maintenance
```

### 4. Frame/Mirror Aesthetic (Optional)

**Budget Option:**
- IKEA RIBBA frame (larger sizes)
- Remove glass, mount TV inside
- ~$30-50

**DIY Option:**
- Lumber and corner brackets
- Paint/stain to match decor
- ~$20-40

**Professional:**
- Custom frame shop
- ~$100-200

---

## Part 7: Troubleshooting

### Camera Not Detected

```bash
# List video devices
ls -l /dev/video*

# Should show /dev/video0 (or similar)

# Check camera permissions
sudo usermod -a -G video pi

# Reboot
sudo reboot

# Test camera directly
raspistill -o test.jpg  # For Pi Camera
ffmpeg -i /dev/video0 -frames 1 test.jpg  # For USB camera
```

### Low FPS / Laggy

**Checks:**
```bash
# Monitor CPU usage
htop

# Check temperature
vcgencmd measure_temp
# Should be < 70°C

# Check memory
free -h

# Reduce resolution in code
# Use Standard (10 level) palette
# Lower terminal character count
```

### Service Won't Start

```bash
# Check logs
journalctl -u ascii-mirror.service -n 50

# Test manually
cd ~/opencv2term/build
./OpenCVProject

# Check permissions
ls -l /dev/tty1
sudo chmod 666 /dev/tty1  # If needed
```

### Display Issues

```bash
# Check HDMI connection
tvservice -s

# Force HDMI mode if needed
sudo nano /boot/firmware/config.txt
# Add:
hdmi_force_hotplug=1
hdmi_drive=2
```

---

## Part 8: Maintenance & Updates

### System Updates

```bash
# Monthly updates
sudo apt update
sudo apt upgrade -y

# Reboot after kernel updates
sudo reboot
```

### Project Updates

```bash
cd ~/opencv2term
git pull  # If using git

# Rebuild
cd build
make clean
make -j$(nproc)

# Restart service
sudo systemctl restart ascii-mirror.service
```

### Monitoring

```bash
# Check service status
sudo systemctl status ascii-mirror.service

# View live logs
journalctl -u ascii-mirror.service -f

# Check temperature
watch -n 1 vcgencmd measure_temp

# Check performance
top -d 1
```

---

## Part 9: Performance Benchmarks

### Expected Performance

**Raspberry Pi 5 (8GB):**
```
Resolution: 640x480 camera input
Grid: 100x50 characters
Palette: Standard (10 levels)
FPS: 28-30 (sustained)
CPU: 40-50%
Temperature: 50-60°C (with fan)
Latency: 60-80ms
```

**Raspberry Pi 4 (8GB):**
```
Resolution: 640x480 camera input
Grid: 80x40 characters
Palette: Standard (10 levels)
FPS: 24-28 (sustained)
CPU: 60-70%
Temperature: 55-65°C (with fan)
Latency: 100-120ms
```

### Optimization Tips

1. **Lower Resolution**: 640x480 is optimal
2. **Simple Palette**: Use 10-level, not 70-level
3. **Smaller Grid**: 80x40 instead of 120x60
4. **Active Cooling**: Fan keeps CPU from throttling
5. **Overclock** (Pi 4): Can gain 10-15% performance

---

## Part 10: Advanced Features

### Auto-Sleep When No Motion

Add motion detection to save power when no one is present:

```cpp
// In main.cpp, add motion detection
bool detectMotion(const cv::Mat& frame1, const cv::Mat& frame2) {
    cv::Mat diff;
    cv::absdiff(frame1, frame2, diff);
    int motionPixels = cv::countNonZero(diff > 30);
    return motionPixels > 1000;  // Threshold
}
```

### Time Display Overlay

Add clock to ASCII art display (future enhancement).

### Photo Capture

Add GPIO button to capture ASCII "photos".

---

## Appendix: Quick Reference

### Useful Commands

```bash
# Restart mirror
sudo systemctl restart ascii-mirror.service

# Stop mirror
sudo systemctl stop ascii-mirror.service

# View logs
journalctl -u ascii-mirror.service -f

# Check temperature
vcgencmd measure_temp

# Update project
cd ~/opencv2term && git pull && cd build && make && sudo systemctl restart ascii-mirror.service
```

### Camera Testing

```bash
# Test Pi Camera
libcamera-hello --list-cameras
libcamera-still -o test.jpg

# Test USB Camera
v4l2-ctl --list-devices
ffmpeg -f v4l2 -i /dev/video0 -frames 1 test.jpg
```

### Network Access

```bash
# Find Pi IP
hostname -I

# SSH from another computer
ssh pi@ascii-mirror.local

# VNC (optional)
sudo apt install realvnc-vnc-server
sudo raspi-config  # Interface Options → VNC → Enable
```

---

## Support & Resources

- **Raspberry Pi Forums**: https://forums.raspberrypi.com/
- **OpenCV Documentation**: https://docs.opencv.org/
- **Project GitHub**: [Your repo URL]

---

**Congratulations! Your ASCII Art Digital Mirror is complete!** 🎨🪞✨

