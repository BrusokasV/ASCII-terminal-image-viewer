#include <ncurses/ncurses.h>
#include <stdint.h>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <unistd.h>
#include <filesystem>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void drawUI(const char* path, const char* filename, bool displaying);
void drawDirectory(std::vector<std::string> directory, int highlight);
void drawImage(uint8_t* image, int width, int height);
uint8_t* downscale(uint8_t* init_image, int init_width, int init_height, int* pnew_width, int* pnew_height);

char map[10] = {' ','.',':','-','=','+','*','#','%','@'};
bool mode_contrast = true;
bool mode_inverse = false;

int main(int argc, char *argv[]){
    int width, height, bpp, ds_width, ds_height;
    int ch, opt;

    while ((opt = getopt(argc, argv, "")) != -1){
        switch (opt){
            default:
                std::cerr << "Usage: " << argv[0] << " dir/ \n";
                return 1;
        }
    }
    char* path = argv[optind];

    std::vector<std::string> directory;

    for (const auto &entry : std::filesystem::directory_iterator(path)) {
        if (std::filesystem::is_regular_file(entry.status())) {
            directory.push_back(entry.path().string());
        }
    }
    
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);

    int highlight = 1;
    bool displaying = false;
    bool shouldend = false;
    const char* filename;

    uint8_t* rgb_image;
    uint8_t* ds_image;

    attron(COLOR_PAIR(1));

    drawDirectory(directory, highlight);
    drawUI(path, filename, displaying);

    while (!shouldend) {
        ch = getch();
        switch (ch) {
            case KEY_UP:
                highlight--;
                if (highlight <= 0) highlight = directory.size();
                drawDirectory(directory, highlight);
                drawUI(path, filename, displaying);
                break;
            case KEY_DOWN:
                highlight++;
                if (highlight > (int)directory.size()) highlight = 1;
                drawDirectory(directory, highlight);
                drawUI(path, filename, displaying);
                break;
            case 10:
            case KEY_ENTER:
                clear();
                mvprintw(LINES/2, COLS/2 - 5, "Loading...");
                displaying = true;
                filename = directory[highlight-1].c_str();
                rgb_image = stbi_load(filename, &width, &height, &bpp, 1);
                ds_image = downscale(rgb_image, width, height, &ds_width, &ds_height);
                drawImage(ds_image, ds_width, ds_height);
                drawDirectory(directory, highlight);
                drawUI(path, filename, displaying);
                break;
            case 105:
            case 73:
                mode_inverse = !mode_inverse;
                std::reverse(std::begin(map), std::end(map));
                if (displaying){
                    drawImage(ds_image, ds_width, ds_height);
                    drawDirectory(directory, highlight);
                }
                drawUI(path, filename, displaying);
                break;
            case 99:
            case 67:
                mode_contrast = !mode_contrast;
                if (displaying){
                    drawImage(ds_image, ds_width, ds_height);
                    drawDirectory(directory, highlight);
                }
                drawUI(path, filename, displaying);
                break;
            case 27:
                shouldend = true;
                break;
            default:
                break;
        }
    }
    stbi_image_free(rgb_image);
    stbi_image_free(ds_image);

    attroff(COLOR_PAIR(1));
    
    endwin();
}

void drawUI(const char* path, const char* filename, bool displaying){
    int len = 0;
    if (displaying) len = strlen(filename) + 3;
    else len = strlen(path) + 3;

    for (int i = 0; i < COLS; i++) {
        mvprintw(0, i, "=");
        mvprintw(LINES-1, i, "=");
    }

    const char* c_state;
    const char* i_state;

    if (mode_contrast) c_state = "ON ";
    else c_state = "OFF";

    if (mode_inverse) i_state = "ON ";
    else i_state = "OFF";

    mvprintw(2, COLS - 36, "MODE:");
    mvprintw(3, COLS - 36, "Contrast: %s (press 'c' to toggle)", c_state);
    mvprintw(4, COLS - 36, "Inverse:  %s (press 'i' to toggle)", i_state);

    if (displaying) mvprintw(LINES-1, COLS/2 - len/2, "{/%s}", filename);
    else mvprintw(LINES-1, COLS/2 - len/2, "{/%s}", path);
}

void drawDirectory(std::vector<std::string> directory, int highlight){
    int starty = 2;

    for (auto name : directory){
        if (starty / 2 == highlight) attron(A_REVERSE);
        mvprintw(starty, 2, "[%s]", name.c_str());
        if (starty / 2 == highlight) attroff(A_REVERSE);
        starty+=2;
    }

    refresh();
}

void drawImage(uint8_t* image, int width, int height){
    int startx = COLS/2 - width;
    int starty = LINES/2 - height/2;

    int d_offset = 0;
    int b_scale = 256;
    int v_min = *std::min_element(image, image + (height*width));
    int v_max = *std::max_element(image, image + (height*width))+1;

    float a_scale = 10.0;

    float b_factor = b_scale / a_scale; 
    //mvprintw(2, 0, "d_offset = %d, b_scale = %d, a_scale = %f, b_factor = %f, v_min = %d, v_max = %d", d_offset, b_scale, a_scale, b_factor, v_min, v_max);

    for (int i = 0; i < height; i++){
        move(starty, startx);
        for (int j = 0; j < width; j++){
            int val = 0;
            if (mode_contrast) val = (int)(std::floor((image[i*width + j] - v_min) * a_scale / (v_max - v_min)));
            else val = (int)(std::floor(image[i*width + j] / b_factor));
            printw("%c ", map[val]);
        }
        starty++;
    }
    refresh();
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
    int acc, count;
    if ((init_width == new_width) && (init_height == new_height)){
        for (int target_i = 0; target_i < new_height*new_width; target_i++){
            target_image[target_i] = init_image[target_i];
        }
    }
    else {
#pragma omp parallel for collapse(2) private(acc, count) shared(new_height, new_width, factor, init_image, target_image)
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
    }
    *pnew_height = new_height;
    *pnew_width = new_width;
    return target_image;
}