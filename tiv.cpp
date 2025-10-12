#include <ncurses/ncurses.h>
#include <stdint.h>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

const char map[10] = {' ','.', ':', '-', '=', '+', '*', '#', '%', '@'};

int main(){
    initscr();
    int width, height, bpp, mapped;
    uint8_t* rgb_image = stbi_load("images/arasaka.png", &width, &height, &bpp, 1);
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            float f = std::lerp(0, 9, (rgb_image[i*width + j] / 255));
            printw("%c ", map[(int)(std::floor(rgb_image[i*width + j] / 25.6))]);
        }
        printw("\n");
    }
    refresh();
    getch();
    stbi_image_free(rgb_image);
    endwin();
}
