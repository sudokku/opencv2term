#include <iostream>
#include <opencv2/opencv.hpp>
#include <ncurses.h>
#include <csignal>

using std::cout;
using std::endl;
using namespace cv;

char ascii_gradient[16] = {
    ' ',
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
    '@'};

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

// Print OpenCV::image to terminal using Ncurses
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
    clear(); // Clear the screen to prevent artifacts
    for (int i = 0; i < resized_image.rows; i++)
    {
        for (int j = 0; j < resized_image.cols; j++)
        {
            mvprintw(i, j, "%c", ascii_matrix[i * resized_image.cols + j]); // Print character at specific location
        }
    }
    // printw("Window size %d x %d", max_x, max_y);
    refresh(); // Refresh the screen to show changes
}

// Global variables for handling resize
Mat global_image;
Mat global_clamped_image;

// Signal handler for window resize
void handleResize(int sig)
{
    // Recalculate and redraw the image
    if (!global_clamped_image.empty())
    {
        printImage(global_clamped_image);
    }
}

int main(int argc, char **argv)
{
    // Load the grayscale image
    global_image = imread("../in-image.jpg", IMREAD_GRAYSCALE);

    if (global_image.empty())
    {
        cout << "Not a valid image file" << endl;
        return -1;
    }

    // Create second image using the grayscale image, clamp the pixel values to the range 0-15
    global_clamped_image = image_pixel_clamp(global_image, 0, 15);

    // Start Ncurses
    initscr();
    noecho();        // Disable echoing of user input
    curs_set(FALSE); // Hide the cursor

    // Set up resize signal handler
    signal(SIGWINCH, handleResize);

    // Main loop
    for (;;)
    {
        handleResize(0); // Initial drawing
        char c = getch();
        if (c == '\n') // Exit on ENTER key
        {
            endwin();
            break;
        }
    }

    return 0;
}
