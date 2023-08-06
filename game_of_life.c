#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define HEIGHT 25
#define WIDTH 80
#define ARCH 100
#define ALIVE '*'
#define DEAD '-'
#define BLUE "\x1b[44m"
#define PURPLE "\x1b[35m"
#define RESET "\x1b[0m"
#define PAUSE 1000000
#define MAX_FILE_NAME 100

void initial_matrix(int matrix[HEIGHT][WIDTH], FILE* file);
void game_start(FILE* file, int speed);
void print(int matrix[HEIGHT][WIDTH]);
void matrix_update(int matrix[HEIGHT][WIDTH]);
int cell_update(int matrix[HEIGHT][WIDTH], int x, int y, int old_status);
void record_matrix(int matrix[HEIGHT][WIDTH], int archive[ARCH][HEIGHT][WIDTH], int *count);
int game_end(int matrix[HEIGHT][WIDTH], int archive[ARCH][HEIGHT][WIDTH]);

int main() {
    char file_name[MAX_FILE_NAME];
    int speed;
    printf("Choose game speed (1 - min, 5 - max): ");
    scanf("%d", &speed);
    speed = PAUSE / speed;
    printf("Select file to play: ");
    scanf("%s", file_name);
    FILE *file;
    file = fopen(file_name, "r");
    game_start(file, speed);
    fclose(file);
    return 0;
}

void game_start(FILE* file, int speed) {
    int count = 0;
    int matrix[HEIGHT][WIDTH];
    int archive[ARCH][HEIGHT][WIDTH];
    for (int a = 0; a < ARCH; a++) {
        for (int i = 0; i < HEIGHT; i++) {
            for (int j = 0; j < WIDTH; j++) {
                archive[a][i][j] = 0;
            }
        }
    }
    initial_matrix(matrix, file);
    while (game_end(matrix, archive)) {
        printf("\e[1;1H\e[2J");
        print(matrix);
        record_matrix(matrix, archive, &count);
        matrix_update(matrix);
        usleep(speed);
    }
    print(matrix);
}

void initial_matrix(int matrix[HEIGHT][WIDTH], FILE* file) {
    char c;
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (fscanf(file, "%c", &c) != 1 || (c != ALIVE && c != DEAD)) {
                printf("Error!%c is incorrect input!\n", c);
                exit(0);
            };
            if (c == ALIVE) {
                matrix[i][j] = 1;
            } else
                matrix[i][j] = 0;
        }

        if ((fscanf(file, "%c", &c) != 1 || c != '\n') && (i < HEIGHT - 1)) {
            printf("Error!%c is incorrect input!\n", c);
            exit(0);
        }
    };
}

void print(int matrix[HEIGHT][WIDTH]) {
    for (int i = 0; i < WIDTH + 2; i++) {
        printf("%s%c%s", BLUE, '.', RESET);
    }
    printf("\n");
    for (int i = 0; i < HEIGHT; i++) {
        printf("%s%c%s", BLUE, '.', RESET);
        for (int j = 0; j < WIDTH; j++) {
            if (matrix[i][j])
                printf("%s%c%s", PURPLE, '*', RESET);
            else
                printf("-");
        }
        printf("%s%s%s", BLUE, ".\n", RESET);
    }
    for (int i = 0; i < WIDTH + 2; i++) {
        printf("%s%c%s", BLUE, '.', RESET);
    }
    printf("\n");
}

void matrix_update(int matrix[HEIGHT][WIDTH]) {
    int old[HEIGHT][WIDTH];
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            old[i][j] = matrix[i][j];
        }
    }
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            matrix[i][j] = cell_update(old, j, i, old[i][j]);
        }
    }
}

int cell_update(int matrix[HEIGHT][WIDTH], int x, int y, int old_status) {
    int sum = 0;
    int new_status = 0;
    for (int i = y - 1; i < y + 2; i++) {
        for (int j = x - 1; j < x + 2; j++) {
            sum += plato[(i + HEIGHT) % HEIGHT][(j + WIDTH) % WIDTH];
        }
    }
    sum -= matrix[y][x];
    if (old_status) {
        if (sum == 2 || sum == 3) new_status = 1;
    } else {
        if (sum == 3) new_status = 1;
    }
    return new_status;
}

void record_matrix(int matrix[HEIGHT][WIDTH], int archive[ARCH][HEIGHT][WIDTH], int *count) {
    if (*count >= ARCH) *count = 0;
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            archive[*count][i][j] = matrix[i][j];
        }
    }
    *count = *count + 1;
}

int game_end(int matrix[HEIGHT][WIDTH], int archive[ARCH][HEIGHT][WIDTH]) {
    int cycling;
    for (int a = 0; a < ARCH; a++) {
        cycling = 0;
        for (int i = 0; i < HEIGHT; i++) {
            for (int j = 0; j < WIDTH; j++) {
                if (matrix[i][j] != archive[a][i][j]) {
                    cycling = 1;
                }
            }
        }
        if (!cycling) break;
    }
    return cycling;
}
