// opencv-test.cpp

#include <iostream>
#include <opencv2/opencv.hpp>

using std::cout;
using std::endl;
using namespace cv;

int main(int argc, char **argv)
{
    Mat image = imread("../my-image.jpg", IMREAD_GRAYSCALE);

    if (image.empty())
    {
        cout << "Not a valid image file" << endl;
        return -1;
    }

    namedWindow("Simple Demo", WINDOW_AUTOSIZE);
    imshow("Simple Demo", image);

    waitKey(0);
    destroyAllWindows();

    return 0;
}