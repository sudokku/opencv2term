#include "VideoProcessor.h"
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

VideoProcessor::VideoProcessor() 
    : frameCount_(0), fps_(0), width_(0), height_(0) {}

VideoProcessor::~VideoProcessor() {
    release();
}

bool VideoProcessor::loadVideo(const std::string& filepath) {
    capture_.open(filepath);
    
    if (!capture_.isOpened()) {
        return false;
    }
    
    // Get video properties
    frameCount_ = static_cast<int>(capture_.get(cv::CAP_PROP_FRAME_COUNT));
    fps_ = capture_.get(cv::CAP_PROP_FPS);
    width_ = static_cast<int>(capture_.get(cv::CAP_PROP_FRAME_WIDTH));
    height_ = static_cast<int>(capture_.get(cv::CAP_PROP_FRAME_HEIGHT));
    
    return true;
}

std::vector<std::string> VideoProcessor::getVideoFiles(const std::string& directory) {
    std::vector<std::string> videoFiles;
    std::vector<std::string> extensions = {".mp4", ".avi", ".mov", ".mkv", ".wmv", ".flv", ".webm"};
    
    try {
        for (const auto& entry : fs::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                std::string extension = entry.path().extension().string();
                
                // Convert extension to lowercase
                std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
                
                // Check if file has a video extension
                if (std::find(extensions.begin(), extensions.end(), extension) != extensions.end()) {
                    videoFiles.push_back(filename);
                }
            }
        }
    } catch (const fs::filesystem_error&) {
        return videoFiles;
    }
    
    // Sort alphabetically
    std::sort(videoFiles.begin(), videoFiles.end());
    
    return videoFiles;
}

bool VideoProcessor::getNextFrame(cv::Mat& frame) {
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

void VideoProcessor::reset() {
    if (capture_.isOpened()) {
        capture_.set(cv::CAP_PROP_POS_FRAMES, 0);
    }
}

void VideoProcessor::release() {
    if (capture_.isOpened()) {
        capture_.release();
    }
    cv::setNumThreads(0);
}

