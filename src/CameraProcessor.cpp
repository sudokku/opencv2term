#include "CameraProcessor.h"

CameraProcessor::CameraProcessor() 
    : fps_(0), width_(0), height_(0) {}

CameraProcessor::~CameraProcessor() {
    release();
}

bool CameraProcessor::openCamera(int deviceId) {
    // Try to open the camera
    capture_.open(deviceId);
    
    if (!capture_.isOpened()) {
        return false;
    }
    
    // Get camera properties
    fps_ = capture_.get(cv::CAP_PROP_FPS);
    width_ = static_cast<int>(capture_.get(cv::CAP_PROP_FRAME_WIDTH));
    height_ = static_cast<int>(capture_.get(cv::CAP_PROP_FRAME_HEIGHT));
    
    // If FPS is 0 or invalid, set a default
    if (fps_ <= 0 || fps_ > 120) {
        fps_ = 30.0;  // Default to 30 FPS for cameras
    }
    
    return true;
}

bool CameraProcessor::getNextFrame(cv::Mat& frame) {
    if (!capture_.isOpened()) {
        return false;
    }
    
    cv::Mat colorFrame;
    if (!capture_.read(colorFrame)) {
        return false;
    }
    
    // Convert to grayscale
    cv::cvtColor(colorFrame, frame, cv::COLOR_BGR2GRAY);
    
    return true;
}

double CameraProcessor::getFPS() const {
    return fps_;
}

int CameraProcessor::getWidth() const {
    return width_;
}

int CameraProcessor::getHeight() const {
    return height_;
}

void CameraProcessor::release() {
    if (capture_.isOpened()) {
        capture_.release();
    }
}

