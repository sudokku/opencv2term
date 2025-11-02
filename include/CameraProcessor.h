#ifndef CAMERA_PROCESSOR_H
#define CAMERA_PROCESSOR_H

#include <opencv2/opencv.hpp>
#include <string>

class CameraProcessor {
public:
    CameraProcessor();
    ~CameraProcessor();
    
    // Open camera by device ID (0 = default camera)
    bool openCamera(int deviceId = 0);
    
    // Get the next frame from camera (returns false on error)
    bool getNextFrame(cv::Mat& frame);

    // Get the next frame with color from camera (returns false on error)
    bool getNextFrameWithColor(cv::Mat &grayFrame, cv::Mat &colorFrame);

    // Get camera properties
    double getFPS() const;
    int getWidth() const;
    int getHeight() const;
    
    // Check if camera is opened
    bool isOpened() const { return capture_.isOpened(); }
    
    // Cleanup
    void release();
    
private:
    cv::VideoCapture capture_;
    double fps_;
    int width_;
    int height_;
};

#endif // CAMERA_PROCESSOR_H

