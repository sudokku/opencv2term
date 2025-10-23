#include <iostream>
#include <opencv2/opencv.hpp>
#include <ncurses.h>
#include <csignal>
#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <unistd.h>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using namespace cv;
namespace fs = std::filesystem;

// ASCII Palette structure
struct AsciiPalette
{
    string name;
    string characters;
    int size;

    AsciiPalette(const string &n, const string &chars)
        : name(n), characters(chars), size(chars.length()) {}
};

// Define multiple ASCII palettes
// Based on research: 10 chars = simple, 20 chars = balanced, 70 chars = detailed
vector<AsciiPalette> palettes = {
    AsciiPalette("Standard (10 levels)", " .:-=+*#%@"),
    AsciiPalette("Detailed (70 levels)", " .'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$"),
    AsciiPalette("Balanced (20 levels)", " .':,;!/>+=*oahkO0Q#MW@")};

// Currently selected palette (global)
const AsciiPalette *selected_palette = nullptr;

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
    if (!selected_palette)
        return;

    for (int i = 0; i < image.rows; i++)
    {
        for (int j = 0; j < image.cols; j++)
        {
            int pixel_value = image.at<uchar>(i, j);
            outMatrix[i * image.cols + j] = selected_palette->characters[pixel_value];
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

// Interactive menu to select an ASCII palette
int selectPaletteMenu()
{
    if (palettes.empty())
    {
        return -1;
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
        mvprintw(1, 2, "Select an ASCII palette:");
        attroff(A_BOLD);

        mvprintw(2, 2, "Use UP/DOWN arrows to navigate, ENTER to select, Q to quit");

        // Print palette list with preview
        for (size_t i = 0; i < palettes.size(); i++)
        {
            if (i == selectedIndex)
            {
                attron(A_REVERSE); // Highlight selected item
                mvprintw(4 + i * 2, 4, "> %s", palettes[i].name.c_str());
                attroff(A_REVERSE);
            }
            else
            {
                mvprintw(4 + i * 2, 4, "  %s", palettes[i].name.c_str());
            }

            // Show preview of the palette
            string preview = "  Preview: " + palettes[i].characters.substr(0, std::min(40, (int)palettes[i].characters.length()));
            mvprintw(5 + i * 2, 6, "%s", preview.c_str());
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
            if (selectedIndex < palettes.size() - 1)
            {
                selectedIndex++;
            }
            break;

        case '\n': // Enter key
        case KEY_ENTER:
            return selectedIndex;

        case 'q':
        case 'Q':
            return -1; // User quit
        }
    }
}

// Interactive menu to select display mode
int selectDisplayModeMenu()
{
    int selectedIndex = 0;
    int key;

    // Enable keypad for arrow keys
    keypad(stdscr, TRUE);

    vector<string> modes = {
        "Display in current terminal (interactive, resizable)",
        "Open in new window (static, with calculated dimensions)"};

    while (true)
    {
        clear();

        // Print title
        attron(A_BOLD);
        mvprintw(1, 2, "Select display mode:");
        attroff(A_BOLD);

        mvprintw(2, 2, "Use UP/DOWN arrows to navigate, ENTER to select, Q to quit");

        // Print display mode list
        for (size_t i = 0; i < modes.size(); i++)
        {
            if (i == selectedIndex)
            {
                attron(A_REVERSE); // Highlight selected item
                mvprintw(4 + i, 4, "> %s", modes[i].c_str());
                attroff(A_REVERSE);
            }
            else
            {
                mvprintw(4 + i, 4, "  %s", modes[i].c_str());
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
            if (selectedIndex < modes.size() - 1)
            {
                selectedIndex++;
            }
            break;

        case '\n': // Enter key
        case KEY_ENTER:
            return selectedIndex;

        case 'q':
        case 'Q':
            return -1; // User quit
        }
    }
}

// Generate ASCII art to a file with calculated dimensions
void generateAsciiToFile(Mat image, const string &filename, int maxWidth, int maxHeight)
{
    // Calculate dimensions preserving aspect ratio
    double aspectRatio = (double)image.cols / image.rows;

    // Account for terminal character aspect ratio (chars are taller than wide)
    // Typical terminal char is ~2:1 height:width ratio
    aspectRatio *= 2.0;

    int width, height;

    if (aspectRatio > ((double)maxWidth / maxHeight))
    {
        // Width is the limiting factor
        width = maxWidth;
        height = (int)(width / aspectRatio);
    }
    else
    {
        // Height is the limiting factor
        height = maxHeight;
        width = (int)(height * aspectRatio);
    }

    // Ensure minimum dimensions
    if (width < 10)
        width = 10;
    if (height < 10)
        height = 10;

    // Resize image
    Mat resized_image;
    resize(image, resized_image, Size(width, height));

    // Generate ASCII art
    char ascii_matrix[resized_image.rows * resized_image.cols];
    image_to_ascii(resized_image, ascii_matrix);

    // Write to file
    std::ofstream outFile(filename);
    if (outFile.is_open())
    {
        for (int i = 0; i < resized_image.rows; i++)
        {
            for (int j = 0; j < resized_image.cols; j++)
            {
                outFile << ascii_matrix[i * resized_image.cols + j];
            }
            outFile << '\n';
        }
        outFile.close();
    }
}

// Open ASCII art in a new terminal window (macOS)
void openInNewWindow(const string &filename)
{
    // Create AppleScript command to open new Terminal window with the file
    string command = "osascript -e 'tell application \"Terminal\" to do script \"clear && cat " +
                     filename + " && echo \\\"\\\\nPress any key to close...\\\" && read -n 1\"'";
    system(command.c_str());
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

    // Start Ncurses for the menus
    initscr();
    noecho();        // Disable echoing of user input
    curs_set(FALSE); // Hide the cursor

    // Show palette selection menu first
    int paletteIndex = selectPaletteMenu();

    // Check if user quit
    if (paletteIndex < 0)
    {
        endwin();
        cout << "No palette selected. Exiting..." << endl;
        return 0;
    }

    // Set the selected palette
    selected_palette = &palettes[paletteIndex];

    // Show image selection menu
    string selectedImage = selectImageMenu(imageFiles);

    // Check if user quit
    if (selectedImage.empty())
    {
        endwin();
        cout << "No image selected. Exiting..." << endl;
        return 0;
    }

    // Show display mode selection menu
    int displayMode = selectDisplayModeMenu();

    // Check if user quit
    if (displayMode < 0)
    {
        endwin();
        cout << "No display mode selected. Exiting..." << endl;
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

    // Create second image using the grayscale image, clamp the pixel values based on palette size
    global_clamped_image = image_pixel_clamp(global_image, 0, selected_palette->size - 1);

    if (displayMode == 0)
    {
        // Mode 0: Display in current terminal (interactive)

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
    }
    else if (displayMode == 1)
    {
        // Mode 1: Open in new window (static)

        // Max dimensions for new window (reasonable defaults)
        int maxWidth = 120;
        int maxHeight = 60;

        // Generate ASCII art to a temporary file
        string tempFile = "/tmp/opencv2term_output.txt";
        generateAsciiToFile(global_clamped_image, tempFile, maxWidth, maxHeight);

        cout << "Opening ASCII art in new window..." << endl;
        cout << "Dimensions: max " << maxWidth << "x" << maxHeight << " (aspect ratio preserved)" << endl;

        // Open in new terminal window
        openInNewWindow(tempFile);

        // Wait a moment for the window to open
        sleep(1);
    }

    // Explicitly release OpenCV Mat objects
    global_image.release();
    global_clamped_image.release();

    // Clean up OpenCV's thread pool
    cv::setNumThreads(0);

    return 0;
}
