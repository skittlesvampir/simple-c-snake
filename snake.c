#define _DEFAULT_SOURCE // gcc gibt sonst error wegen usleep
#include <stdio.h>
#include <unistd.h> // sleep
#include <stdbool.h> // boolean
#include <termios.h>
#include <stdlib.h> // atexit, rand
#include <sys/ioctl.h> // FIONREAD
#include <time.h> // for RNG initialization
#include <string.h> // strtok
#include "pasted-code.h"

void textcolor (int bg) {
    printf("\033[%dm", bg);
}

void setPixel (int column, int line, int color) {
    textcolor(color);
    printf("\033[%d;%dH  ", line, column);
    textcolor(0);
    fflush(stdout);
}

void makeBorder (int column, int line) {
    setPixel(2*column+1, line, 45);
}

void setCursor (int column, int line) {
    setPixel(2*column+3, line+2, 43);
}

void setHead (int column, int line) {
    setPixel(2*column+3, line+2, 44);
}

void clearCursor (int column, int line) {
    setPixel(2*column+3, line+2, 0);
}

void makeApple (int column, int line) {
    setPixel(2*column+3, line+2, 46);
}

void makeRed (int column, int line) {
    setPixel(2*column+3, line+2, 41);
}

void drawField (int x, int y) {
    int i;
    int ii;
    printf("\033[?25l"); // Hide cursor
    printf("\033[2J\033[H"); // Clear screen
    for(i=0;i<x;i++){
        makeBorder(i, 0);
    }
    for (i=1;i<y;i++) {
        makeBorder(0, i);
        for (ii=0; ii<x-2; ii++) {
            clearCursor(ii, i);
        }
        makeBorder(x-1, i);
    }
    for (i=0; i<x; i++) {
        makeBorder(i, y);
    }
}

void spawnApple (int field_size_x, int field_size_y, bool field[field_size_x][field_size_y], int *apple_pos_x, int *apple_pos_y) {
    int rand_pos_x;
    int rand_pos_y;
    while (1) {
        rand_pos_x = rand() % (field_size_x-1);
        rand_pos_y = rand() % (field_size_y-1);
        if (!field[rand_pos_x][rand_pos_y]) {
            break;
        }
    }
    makeApple(rand_pos_x, rand_pos_y);
    *apple_pos_x = rand_pos_x;
    *apple_pos_y = rand_pos_y;
}

void updateScore (int score, int field_size_y) {
    printf("\033[%d;0HScore: %d\n", field_size_y, score);
}

bool checkGameOver (int field_size_x, int field_size_y, bool field[field_size_x][field_size_y], int *head_pos) {
    if (field[head_pos[0]][head_pos[1]]) {
        return true;
    } else {
        return false;
    }
}

bool doGameOver (int field_size_x, int field_size_y, bool field[field_size_x][field_size_y]) {
    int i;
    int ii;
    for (i=0; i<field_size_x; i++) {
        for (ii=0; ii<field_size_y; ii++) {
            if (field[i][ii]) {
                makeRed(i,ii);
            }
        }
    }
    usleep(300000);
    char c;
    while (kbhit()) { read(STDIN_FILENO, &c, 1); } // Flush keys that have been
                            // pressed during the pause so that keypressed that
                            // are still from the game won't be registered as
                            // an exit signal
    printf("\033[%d;%dHGame Over!\nPress space to restart or any other key to exit...", field_size_y+4, 0);
    return getchar() == ' ';
    getchar(); // Wait until the any key has been pressed
}

void pauseGame (int field_size_y) {
    printf("\033[%d;%dHGame paused!\nPress space to resume...", field_size_y+4, 0);
    while (getchar() != ' ');
    printf("\033[2K\033[1A\033[2K"); // erease two the two lines that have been printed before
}

bool game(int field_size_x, int field_size_y) {
    int delay = 2000;
    field_size_x += 2;
    field_size_y += 2;
    drawField(field_size_x, field_size_y);
    field_size_x -= 2;
    field_size_y -= 2;
    int i;
    int ii;
    bool field[field_size_x][field_size_y];
    for (i=0; i < field_size_x; i++) {
        for (ii=0; ii < field_size_y; ii++) {
            field[i][ii] = false;
        }
    }
    int snake_pixels[field_size_x*field_size_y][2];
    char c;
    int length = 4;
    int head_pos[] = {length,field_size_y/2};
    setHead(head_pos[0],head_pos[1]);
    int old_direction;
    int direction = 2;
    for (i=0;i<length;i++) {
        setCursor(i,3);
        field[i][3] = true;
        snake_pixels[i][0] = i;
        snake_pixels[i][1] = 3;
    }
    int apple_pos_x;
    int apple_pos_y;
    srand(time(0));
    spawnApple(field_size_x, field_size_y, field, &apple_pos_x, &apple_pos_y);
    int score = 0;
    int counter = 0;
    while (1) {
        old_direction = direction;
        counter++;
        if (kbhit()) {
            c = '\0';
            read(STDIN_FILENO, &c, 1);
            // printf("%d ('%c')\n", c, c);
            switch (c) {
                case 27: 
                    if (c == 27){
                        read(STDIN_FILENO, &c, 1);
                        if (c == 91) {
                            read(STDIN_FILENO, &c, 1);
                            switch (c) {
                                case 65:
                                    if (direction != 1){
                                        direction = 0; // oben
                                    }
                                    break;
                                case 66:
                                    if (direction != 0){
                                        direction = 1; // unten
                                    }
                                    break;
                                case 67:
                                    if (direction != 3){
                                        direction = 2; // rechts
                                    }
                                    break;
                                case 68:
                                    if (direction != 2){
                                        direction = 3; // links
                                    }
                            }
                        }
                    }
                    break;
                case ' ':
                    pauseGame(field_size_y);
                    break;
                case 'w': // WASD
                case 'k': // vi keybindings
                    if (direction != 1) {
                        direction = 0;
                    }
                    break;
                case 's':
                case 'j':
                    if (direction != 0) {
                        direction = 1;
                    }
                    break;
                case 'd':
                case 'l':
                    if (direction != 3) {
                        direction = 2;
                    }
                    break;
                case 'a':
                case 'h':
                    if (direction != 2) {
                        direction = 3;
                    }
            }
            if (old_direction == direction) {
                continue;
            }
            counter = 0;
        } else {
            if (counter < 100) {
                usleep(delay);
                continue;
            } else {
                counter = 0;
            }
        }
        setCursor(head_pos[0], head_pos[1]);
        clearCursor(snake_pixels[0][0], snake_pixels[0][1]);
        field[snake_pixels[0][0]][snake_pixels[0][1]] = false;
        snake_pixels[length][0] = head_pos[0];
        snake_pixels[length][1] = head_pos[1];
        field[head_pos[0]][head_pos[1]] = true;
        for (i=1;i<length+1;i++){
            snake_pixels[i-1][0] = snake_pixels[i][0];
            snake_pixels[i-1][1] = snake_pixels[i][1];
        }
        if (head_pos[0] == apple_pos_x && head_pos[1] == apple_pos_y) {
            length++;
            score++;
            spawnApple(field_size_x, field_size_y, field, &apple_pos_x, &apple_pos_y);
        }
        updateScore(score, field_size_y+3);
        switch (direction) {
            case 0:
                if (head_pos[1] > 0) {
                    head_pos[1]--;
                }
                break;
            case 1:
                if (head_pos[1] < field_size_y-1) {
                    head_pos[1]++;
                }
                break;
            case 2:
                if (head_pos[0] < field_size_x-1) {
                    head_pos[0]++;
                }
                break;
            case 3:
                if (head_pos[0] > 0) {
                    head_pos[0]--;
                }
                break;
        }
        if (checkGameOver(field_size_x, field_size_y, field, head_pos)) {
            return doGameOver(field_size_x, field_size_y, field);
        }
        setHead(head_pos[0], head_pos[1]);
        usleep(delay);
    }
}

int main(int argc, char *argv[]) {
    enableRawMode();
    int field_size_x = 16;
    int field_size_y = 8;
    if (argc >= 2) {
        sscanf(argv[1], "%dx%d", &field_size_x, &field_size_y);
        if (field_size_x < 5 || field_size_y < 5) {
            printf("Field size must be at least 5x5!\n");
            return 1;
        }
        if (field_size_x > 41 || field_size_y > 41) {
            printf("Field size mustn't be bigger than 41x41!\n");
            return 1;
        }
    }
    while (game(field_size_x, field_size_y));
    printf("\033[H"); // Move cursor to home positon (0,0)
    printf("\033[2J"); // Erease entire screen
    printf("\033[?25h"); // Show cursor
    return 0;
}
