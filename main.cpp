// opencv-test.cpp

#include <iostream>
#include <opencv2/opencv.hpp>
#include <ncurses.h>

using std::cout;
using std::endl;
using namespace cv;

char ascii_gradient[16] = {
    ' ', // Darkest
    '\'',
    ',',
    '^',
    '.',
    '-',
    '+',
    '=',
    '%',
    ':',
    '*',
    'o',
    '&',
    '8',
    '#',
    '@' // Lightest
};

// Maps the pixel values of an image to a new range
Mat image_pixel_clamp(Mat image, int min, int max)
{
    Mat new_image = image.clone();

    for (int i = 0; i < image.rows; i++)
    {
        for (int j = 0; j < image.cols; j++)
        {
            new_image.at<uchar>(i, j) = saturate_cast<uchar>(min + (max - min) * (image.at<uchar>(i, j) / 255.0));
        }
    }

    return new_image;
}

// Create matrix of ASCII characters from image pixel values
void image_to_ascii(Mat image, char *outMatrix)
{
    for (int i = 0; i < image.rows; i++)
    {
        for (int j = 0; j < image.cols; j++)
        {
            outMatrix[i * image.cols + j] = ascii_gradient[image.at<uchar>(i, j)];
        }
    }
}

// Print opencv::image to terminal using ncurses
void printImage(Mat image)
{
    Mat resized_image;
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    // Resize image to fit the terminal window
    resize(image, resized_image, Size(max_x, max_y));

    // Create matrix of ASCII characters from image pixel values
    char ascii_matrix[resized_image.rows * resized_image.cols];
    image_to_ascii(resized_image, ascii_matrix);

    // Print the ASCII matrix to the terminal
    for (int i = 0; i < resized_image.rows; i++)
    {
        for (int j = 0; j < resized_image.cols; j++)
        {
            printw("%c", ascii_matrix[i * resized_image.cols + j]);
        }
    }
}

int main(int argc, char **argv)
{
    Mat image = imread("../in-image.jpg", IMREAD_GRAYSCALE);

    if (image.empty())
    {
        cout << "Not a valid image file" << endl;
        return -1;
    }

    // Create second image using the grayscale image, clamp the pixel values to the range 0-15
    Mat clamped_image = image_pixel_clamp(image, 0, 15);

    // Start ncurses and get the window size
    initscr();

    // Wait for user to input 'ENTER' and end ncurses
    for (;;)
    {
        refresh();
        // Print the grayscale clamped image to terminal
        printImage(clamped_image);
        char c = getch();
        if (c == '\n')
        {
            endwin();
            break;
        }
    }

    return 0;
}