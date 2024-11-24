#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define BACKGROUND "\033[48;5;31m" 
#define FOREGROUND "\033[36m"
#define RESET "\e[0m"

int r = 0;
int g = 200;
int b = 200;

char* foreground_col() {
    static char temp[24];
    sprintf(temp, "\e[38;2;%d;%d;%dm", r, g, b);
    return temp;
}

char* background_col() {
    static char temp[24];
    sprintf(temp, "\e[48;2;%d;%d;%dm", r, g, b);
    return temp;
}

uint8_t digits[] = {
    // 0
    0b11110110,
    0b11011110,

    // 1
    0b01100100,
    0b10010010,

    // 2
    0b11100111,
    0b11001110,

    //3
    0b11100111,
    0b10011110,

    //4
    0b10010011,
    0b10010010,

    //5
    0b11110011,
    0b10011110,

    //6
    0b10010011,
    0b11011110,

    //7
    0b11100100,
    0b10010010,

    //8
    0b11110111,
    0b11011110,

    //9
    0b11110111,
    0b10010010,
};

static inline void moveCursor(int x, int y) {
    printf("\033[%d;%dH", y, x);
}

static inline void clear() {
    printf("\033[2J");
}

void printDigit(uint8_t digit, int x, int y) {
    uint8_t temp = 0x80;
    uint8_t offset = 0;

    moveCursor(x, y);

    for (int j = 0; j < 5; ++j) {
        for (int i = 0; i < 3; ++i) {
            if (temp & digits[digit*2 + offset]) {
                printf("%s", background_col());
            } else {
                printf(RESET);
            }
            printf("  " RESET);
            /* printf("%s", temp & digits[digit*2 + offset] ? rgb_to_ansi256(50, 50, 0)"  "RESET : "  "); */
            temp>>=1;
            if (!temp) {
                ++offset;
                temp = 0x80;
            }
        }
        moveCursor(x, y+j+1);
    }
}

void printColon(int x, int y) {
    moveCursor(x, y-1);
    printf(RESET "    ");
    moveCursor(x, y+1);
    printf(RESET " %s  " RESET " ", background_col());
    moveCursor(x, y+3);
    printf(RESET " %s  " RESET " ", background_col());
}

int hexToDec(char ch) {
    if (ch > 64) return ch - 55;
    return ch - 48;
}

int main(int argc, char** argv) {

    for (int i = 0; i < argc; ++i) {
        if (!strcmp(argv[i], "-c")) {
            if (argc-i < 1) break;
            int len = 0;
            while (argv[i+1][len++] != '\0');
            if (len < 7) break;

            r = 0 + hexToDec(argv[i+1][1])*16;
            r += hexToDec(argv[i+1][2]);

            g = 0 + hexToDec(argv[i+1][3])*16;
            g += hexToDec(argv[i+1][4]);

            b = 0 + hexToDec(argv[i+1][5])*16;
            b += hexToDec(argv[i+1][6]);
            ++i;
        }
    }

    clear();
    while (1) {
        clear();
        time_t t = time(NULL);
        struct tm *tm_info = localtime(&t);

        int hour = tm_info->tm_hour;
        int minute = tm_info->tm_min;

        struct winsize w;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
            perror("ioctl");
            return 1;
        }

        int x = w.ws_col/2-15;
        int y = w.ws_row/2-2;

        printDigit(hour/10, x, y);
        printDigit(hour-hour/10*10, x+8, y);
        printColon(x+14, y);
        printDigit(minute/10, x+18, y);
        printDigit(minute-minute/10*10, x+25, y);
        printf("\e[?25l");
        printf("\n");

        moveCursor(x+10, y+6);
        printf("%s%d %d %d\n" RESET, foreground_col(), tm_info->tm_year+1900, tm_info->tm_mon+1, tm_info->tm_mday);

        usleep(1000000);
    }

    return 0;
}
