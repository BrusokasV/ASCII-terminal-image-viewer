#include <ncurses/ncurses.h>
#include <stdint.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


int main(){
    initscr();
    int width, height, bpp;
    uint8_t* rgb_image = stbi_load("images/image.png", &width, &height, &bpp, 1);
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            printw("%d", rgb_image[i*width + j]);
        }
        printw("\n");
    }
    refresh();
    getch();
    stbi_image_free(rgb_image);
    endwin();
}
