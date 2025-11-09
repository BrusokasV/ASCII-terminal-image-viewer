## Description
This is project is able to display png images as ASCII art in the terminal.

## Current functionality:
- Reads in a directory and displays its contents
- When a file is selected, reads in a png file
- Downscales the image in parallel to fit within the terminal
- Displays the image as ASCII art

## ToDo:
- Implement viewing gifs/videos, other image formats

## Usage:
Dependencies:
- ncurses
- stb_image.h

Compilation:
- g++ -fopenmp tiv.cpp -o tiv -lncursesw -std=c++20

Operation:
- ./tiv \<directory/>

- use arrow up, arrow down to navigate the directory, enter to open a file
- use 'c' and 'i' to toggle contrast and inverse of the image, respectively
- use escape to close the viewer
