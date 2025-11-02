#ifndef VIDEO_PROCESSOR_H
#define VIDEO_PROCESSOR_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

class VideoProcessor {
public:
    VideoProcessor();
    ~VideoProcessor();
    
    // Load a video from file
    bool loadVideo(const std::string& filepath);
    
    // Get list of video files from directory
    static std::vector<std::string> getVideoFiles(const std::string& directory);
    
    // Get the next frame (returns false when video ends)
    bool getNextFrame(cv::Mat& frame);
    
    // Reset to beginning of video
    void reset();
    
    // Get video properties
    int getFrameCount() const { return frameCount_; }
    double getFPS() const { return fps_; }
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    
    // Check if video is loaded
    bool isLoaded() const { return capture_.isOpened(); }
    
    // Cleanup
    void release();
    
private:
    cv::VideoCapture capture_;
    int frameCount_;
    double fps_;
    int width_;
    int height_;
};

#endif // VIDEO_PROCESSOR_H

