cmake_minimum_required(VERSION 3.10)
project(OpenCVProject)

set(CMAKE_CXX_STANDARD 17)

# Find OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Find Ncurses
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIRS})

# Add the executable
add_executable(OpenCVProject main.cpp)

# Link OpenCV and Ncurses
target_link_libraries(OpenCVProject ${OpenCV_LIBS} ${CURSES_LIBRARIES})
