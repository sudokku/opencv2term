#include "ImageProcessor.h"
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

ImageProcessor::ImageProcessor() {}

ImageProcessor::~ImageProcessor() {
    release();
}

bool ImageProcessor::loadImage(const std::string& filepath) {
    image_ = cv::imread(filepath, cv::IMREAD_GRAYSCALE);
    return !image_.empty();
}

std::vector<std::string> ImageProcessor::getImageFiles(const std::string& directory) {
    std::vector<std::string> imageFiles;
    std::vector<std::string> extensions = {".jpg", ".jpeg", ".png", ".bmp", ".gif", ".tiff", ".tif"};
    
    try {
        for (const auto& entry : fs::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                std::string extension = entry.path().extension().string();
                
                // Convert extension to lowercase
                std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
                
                // Check if file has an image extension
                if (std::find(extensions.begin(), extensions.end(), extension) != extensions.end()) {
                    imageFiles.push_back(filename);
                }
            }
        }
    } catch (const fs::filesystem_error&) {
        return imageFiles;
    }
    
    // Sort alphabetically
    std::sort(imageFiles.begin(), imageFiles.end());
    
    return imageFiles;
}

cv::Mat ImageProcessor::clampPixelValues(int min, int max) const {
    if (image_.empty()) return cv::Mat();
    
    cv::Mat result = image_.clone();
    
    for (int i = 0; i < result.rows; i++) {
        for (int j = 0; j < result.cols; j++) {
            result.at<uchar>(i, j) = cv::saturate_cast<uchar>(
                min + (max - min) * (image_.at<uchar>(i, j) / 255.0)
            );
        }
    }
    
    return result;
}

cv::Mat ImageProcessor::resizeToFit(int maxWidth, int maxHeight, bool accountForCharAspect) const {
    if (image_.empty()) return cv::Mat();
    
    double aspectRatio = (double)image_.cols / image_.rows;
    
    // Account for terminal character aspect ratio if requested
    if (accountForCharAspect) {
        aspectRatio *= 2.0;  // Terminal chars are ~2:1 height:width
    }
    
    int width, height;
    
    if (aspectRatio > ((double)maxWidth / maxHeight)) {
        width = maxWidth;
        height = (int)(width / aspectRatio);
    } else {
        height = maxHeight;
        width = (int)(height * aspectRatio);
    }
    
    // Ensure minimum dimensions
    if (width < 10) width = 10;
    if (height < 10) height = 10;
    
    cv::Mat resized;
    cv::resize(image_, resized, cv::Size(width, height));
    
    return resized;
}

void ImageProcessor::release() {
    if (!image_.empty()) {
        image_.release();
    }
    cv::setNumThreads(0);  // Clean up OpenCV thread pool
}

