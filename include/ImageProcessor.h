#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

class ImageProcessor {
public:
    ImageProcessor();
    ~ImageProcessor();
    
    // Load an image from file
    bool loadImage(const std::string& filepath);
    
    // Get list of image files from directory
    static std::vector<std::string> getImageFiles(const std::string& directory);
    
    // Clamp pixel values to a specific range
    cv::Mat clampPixelValues(int min, int max) const;
    
    // Resize image to fit dimensions while preserving aspect ratio
    cv::Mat resizeToFit(int maxWidth, int maxHeight, bool accountForCharAspect = false) const;
    
    // Get current image
    const cv::Mat& getImage() const { return image_; }
    
    // Check if image is loaded
    bool isLoaded() const { return !image_.empty(); }
    
    // Cleanup
    void release();
    
private:
    cv::Mat image_;
};

#endif // IMAGE_PROCESSOR_H

