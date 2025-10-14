## Description
This is project is able to display png images as ASCII art in the terminal.

## Current functionality:
- Reads in a png file
- Downscales the image to fit within the terminal
- Applies a contrast operation
- Displays the image as ASCII art

## ToDo:
- Improve downscaling performance through parallelism
- Implement viewing directories, not just single files
- Implement viewing gifs/videos, other image formats
- Add colour?

## Usage:
Dependencies:
- ncurses
- stb_image.h

Compilation:
- g++ tiv.cpp -o tiv -lncursesw -std=c++20

Operation:
- ./tiv [-c] \<filename\>

Optional flags:
- -c: disable contrast mask
