#include <ncurses/ncurses.h>
#include <stdint.h>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void drawUI(char* filename);
void drawImage(uint8_t* image, int width, int height);
uint8_t* downscale(uint8_t* init_image, int init_width, int init_height, int* pnew_width, int* pnew_height);

const char map[10] = {' ','.', ':', '-', '=', '+', '*', '#', '%', '@'};
bool ds = false;

int main(int argc, char *argv[]){
    if (argc != 2){
        printf("Incorrect number of arguments.");
        return 0;
    }
    char* filename = argv[1];

    int width, height, bpp, ds_width, ds_height;
    int ch;
    uint8_t* rgb_image = stbi_load(filename, &width, &height, &bpp, 1);
    uint8_t* ds_image = downscale(rgb_image, width, height, &ds_width, &ds_height);

    initscr();
    cbreak();
    keypad(stdscr, TRUE);
    
    drawUI(filename);
    drawImage(ds_image, ds_width, ds_height);
    refresh();

    getch();
    stbi_image_free(rgb_image);
    stbi_image_free(ds_image);
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

void drawImage(uint8_t* image, int width, int height){
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

uint8_t* downscale(uint8_t* init_image, int init_width, int init_height, int* pnew_width, int* pnew_height){
    
    int factor = std::ceil((float)std::max(init_height, init_width) / std::min(LINES-2, COLS));
    int new_height = init_height / factor;
    int new_width = init_width / factor;
    if ((init_width <= COLS) && (init_height <= LINES-2)){
        new_height = init_height;
        new_width = init_width;
    }
    uint8_t* target_image = new uint8_t[new_height*new_width];
    int index, acc, count;
    if ((init_width == new_width) && (init_height == new_height)){
        for (int target_i = 0; target_i < new_height; target_i++){
            for (int target_j = 0; target_j < new_width; target_j++){
                index = target_i*new_width + target_j;
                target_image[index] = init_image[index];
            }
        }
    }
    else {
        for (int target_i = 0; target_i < new_height; target_i++){
            for (int target_j = 0; target_j < new_width; target_j++){
                acc = 0;
                count = 0;
                for (int init_i = target_i * factor; init_i < (target_i + 1) * factor; init_i++){
                    for (int init_j = target_j * factor; init_j < (target_j + 1) * factor; init_j++){
                        count++;
                        acc = acc + init_image[init_i*init_width + init_j];
                    }
                }
                target_image[target_i*new_width + target_j] = acc/count;
            }
        }
        ds = true;
    }
    *pnew_height = new_height;
    *pnew_width = new_width;
    return target_image;
}