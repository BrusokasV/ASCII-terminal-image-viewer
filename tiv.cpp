#include <ncurses/ncurses.h>
#include <stdint.h>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void drawUI(char* filename);
void drawImage(uint8_t* image, int width, int height, int bpp);

const char map[10] = {' ','.', ':', '-', '=', '+', '*', '#', '%', '@'};

int main(int argc, char *argv[]){
    if (argc != 2){
        printf("Incorrect number of arguments.");
        return 0;
    }
    char* filename = argv[1];

    int width, height, bpp;
    int ch;
    uint8_t* rgb_image = stbi_load(filename, &width, &height, &bpp, 1);

    initscr();
    cbreak();
    keypad(stdscr, TRUE);

    drawUI(filename);
    drawImage(rgb_image, width, height, bpp);
    refresh();

    getch();
    stbi_image_free(rgb_image);
    endwin();
}

void drawUI(char* filename){
    int len = strlen(filename) + 15;

    for (int i = 0; i < COLS; i++) {
        mvprintw(0, i, "=");
        mvprintw(LINES-1, i, "=");
    }

    mvprintw(LINES-1, COLS/2 - len/2, "{Displaying: %s}", filename);
}

void drawImage(uint8_t* image, int width, int height, int bpp){
    int startx = COLS/2 - width;
    int starty = LINES/2 - height/2;
    for (int i = 0; i < height; i++){
        move(starty, startx);
        for (int j = 0; j < width; j++){
            printw("%c ", map[(int)(std::floor(image[i*width + j] / 25.6))]);
        }
        starty++;
    }
}