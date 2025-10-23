#include <iostream>
#include <opencv2/opencv.hpp>
#include <ncurses.h>
#include <csignal>
#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using namespace cv;
namespace fs = std::filesystem;

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

// Get list of image files from a directory
vector<string> getImageFiles(const string &directory)
{
    vector<string> imageFiles;
    vector<string> extensions = {".jpg", ".jpeg", ".png", ".bmp", ".gif", ".tiff", ".tif"};

    try
    {
        for (const auto &entry : fs::directory_iterator(directory))
        {
            if (entry.is_regular_file())
            {
                string filename = entry.path().filename().string();
                string extension = entry.path().extension().string();

                // Convert extension to lowercase for comparison
                std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

                // Check if the file has an image extension
                if (std::find(extensions.begin(), extensions.end(), extension) != extensions.end())
                {
                    imageFiles.push_back(filename);
                }
            }
        }
    }
    catch (const fs::filesystem_error &e)
    {
        return imageFiles;
    }

    // Sort alphabetically
    std::sort(imageFiles.begin(), imageFiles.end());

    return imageFiles;
}

// Interactive menu to select an image file
string selectImageMenu(const vector<string> &imageFiles)
{
    if (imageFiles.empty())
    {
        return "";
    }

    int selectedIndex = 0;
    int key;

    // Enable keypad for arrow keys
    keypad(stdscr, TRUE);

    while (true)
    {
        clear();

        // Print title
        attron(A_BOLD);
        mvprintw(1, 2, "Select an image to display:");
        attroff(A_BOLD);

        mvprintw(2, 2, "Use UP/DOWN arrows to navigate, ENTER to select, Q to quit");

        // Print image list
        for (size_t i = 0; i < imageFiles.size(); i++)
        {
            if (i == selectedIndex)
            {
                attron(A_REVERSE); // Highlight selected item
                mvprintw(4 + i, 4, "> %s", imageFiles[i].c_str());
                attroff(A_REVERSE);
            }
            else
            {
                mvprintw(4 + i, 4, "  %s", imageFiles[i].c_str());
            }
        }

        refresh();

        // Get user input
        key = getch();

        switch (key)
        {
        case KEY_UP:
            if (selectedIndex > 0)
            {
                selectedIndex--;
            }
            break;

        case KEY_DOWN:
            if (selectedIndex < imageFiles.size() - 1)
            {
                selectedIndex++;
            }
            break;

        case '\n': // Enter key
        case KEY_ENTER:
            return imageFiles[selectedIndex];

        case 'q':
        case 'Q':
            return ""; // User quit
        }
    }
}

// Global variables for handling resize
Mat global_image;
Mat global_clamped_image;
bool is_running = false; // Flag to prevent signal handler from running after exit

// Signal handler for window resize
void handleResize(int sig)
{
    // Don't process if we're not running
    if (!is_running)
    {
        return;
    }

    // Update ncurses to recognize the new terminal size
    endwin();
    refresh();

    // Recalculate and redraw the image
    if (!global_clamped_image.empty())
    {
        printImage(global_clamped_image);
    }
}

int main(int argc, char **argv)
{
    // Get list of image files from the images directory
    string imagesDir = "../images";
    vector<string> imageFiles = getImageFiles(imagesDir);

    if (imageFiles.empty())
    {
        cout << "No image files found in " << imagesDir << " directory" << endl;
        cout << "Please add some image files (.jpg, .png, .bmp, etc.) to the images directory" << endl;
        return -1;
    }

    // Start Ncurses for the menu
    initscr();
    noecho();        // Disable echoing of user input
    curs_set(FALSE); // Hide the cursor

    // Show image selection menu
    string selectedImage = selectImageMenu(imageFiles);

    // Check if user quit
    if (selectedImage.empty())
    {
        endwin();
        cout << "No image selected. Exiting..." << endl;
        return 0;
    }

    // End ncurses temporarily to load the image
    endwin();

    // Construct full path to selected image
    string imagePath = imagesDir + "/" + selectedImage;

    // Load the grayscale image
    global_image = imread(imagePath, IMREAD_GRAYSCALE);

    if (global_image.empty())
    {
        cout << "Error: Could not load image file: " << imagePath << endl;
        return -1;
    }

    // Create second image using the grayscale image, clamp the pixel values to the range 0-15
    global_clamped_image = image_pixel_clamp(global_image, 0, 15);

    // Restart Ncurses for image display
    initscr();
    noecho();        // Disable echoing of user input
    curs_set(FALSE); // Hide the cursor

    // Set up resize signal handler
    signal(SIGWINCH, handleResize);

    // Mark as running (for signal handler safety)
    is_running = true;

    // Initial drawing
    handleResize(0);

    // Main loop - just wait for input, resize handler will redraw automatically
    for (;;)
    {
        char c = getch();
        if (c == '\n') // Exit on ENTER key
        {
            break;
        }
    }

    // Cleanup
    is_running = false;        // Prevent signal handler from accessing freed resources
    signal(SIGWINCH, SIG_DFL); // Restore default signal handler
    endwin();                  // Properly close ncurses

    // Explicitly release OpenCV Mat objects
    global_image.release();
    global_clamped_image.release();

    // Clean up OpenCV's thread pool
    cv::setNumThreads(0);

    return 0;
}
