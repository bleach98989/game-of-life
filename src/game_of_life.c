#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#define HEIGHT 25
#define WIDTH 80
#define BORN 3
#define SURVIVES_MIN 2
#define SURVIVES_MAX 3

void renderingGameName(int speed, int generation);
void editSpeed(int *speed, int button);
void upgradeGameBoard(int **board);
void renderingGameField(int **board, int speed, int generation);
void copyGameBoard(int **board, int **clone);
void upgradeCellStatus(int **board, int *modifiedCell, int count);
void freeMemoryBoard(int **board, int **initBoard, int **lastBoard);
int searchNeighborsBesideCell(int **board, int Y, int X);
int checkGameBoard(int **board, int **clone);
int isWin(int **board, int **lastBoard, int speed, int generation);
int **createGameBoard(int **board);
int isEmptyGameBoard(int **board);
int correctScanf(int *number);
int inputData(int **board);

int main() {
    int **gameBoard = createGameBoard(malloc(HEIGHT * sizeof(int *)));
    int **initialGameBoard = createGameBoard(malloc(HEIGHT * sizeof(int *)));
    int **lastGameBoard = createGameBoard(malloc(HEIGHT * sizeof(int *)));
    if (inputData(gameBoard) && freopen("/dev/tty", "r", stdin)) {
        int generation = 0, speed = 500;
        copyGameBoard(gameBoard, initialGameBoard);
        initscr();
        timeout(speed);
        keypad(stdscr, true);
        do {
            renderingGameField(gameBoard, speed, generation);
            copyGameBoard(gameBoard, lastGameBoard);
            upgradeGameBoard(gameBoard);
            generation += 1;
            editSpeed(&speed, getch());
            clear();
        } while (isWin(gameBoard, lastGameBoard, speed, generation));
        freeMemoryBoard(gameBoard, initialGameBoard, lastGameBoard);
        return 1;
    }
    freeMemoryBoard(gameBoard, initialGameBoard, lastGameBoard);
    printf("[ERROR#304]");
    return 0;
}

void editSpeed(int *speed, int button) {
    if (button == KEY_UP) {
        (*speed) - 50 == 0 ? (*speed = 50) : (*speed -= 50);
    } else if (button == KEY_DOWN) {
        (*speed) + 50 > 2000 ? (*speed = 2000) : (*speed += 50);
    }
    timeout(*speed);
}

void renderingGameName(int speed, int generation) {
    printw("   ---==| The Game of Life |==---        ");
    printw("GENERATION #%d      ", generation);
    printw("SPEED - %dms", speed);
    printw("\n\n");
}

void upgradeGameBoard(int **board) {
    int *modifiedCell = malloc(HEIGHT * WIDTH * 2 * sizeof(int)), count = 0;
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int countAliveCell = searchNeighborsBesideCell(board, y, x);
            if (countAliveCell >= SURVIVES_MIN && countAliveCell <= SURVIVES_MAX) {
                if (board[y][x] == -1 && countAliveCell == BORN) {
                    modifiedCell[count] = y;
                    modifiedCell[count + 1] = x;
                    count += 2;
                }
            } else if (board[y][x] == 1) {
                modifiedCell[count] = y;
                modifiedCell[count + 1] = x;
                count += 2;
            }
        }
    }
    upgradeCellStatus(board, modifiedCell, count);
}

void renderingGameField(int **board, int speed, int generation) {
    char verticalBorders = '|';
    char horizontalBorders = '=';
    char cell = '@';
    for (int y = 0; y < HEIGHT + 2; y++) {
        for (int x = 0; x < WIDTH + 2; x++) {
            if ((x == 0) || (x == WIDTH + 1)) {
                x == 0 ? printw("%c", verticalBorders) : printw("%c\n", verticalBorders);
                continue;
            }
            if (y == 0 || y == HEIGHT + 1) {
                printw("%c", horizontalBorders);
                continue;
            }
            board[y - 1][x - 1] == 1 ? printw("%c", cell) : printw(" ");
        }
    }
    renderingGameName(speed, generation);
    refresh();
}

void copyGameBoard(int **board, int **clone) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            clone[y][x] = board[y][x];
        }
    }
}

void upgradeCellStatus(int **board, int *modifiedCell, int count) {
    for (int q = 0; q < count; q += 2) {
        int Y = modifiedCell[q], X = modifiedCell[q + 1];
        board[Y][X] *= (-1);
    }
    free(modifiedCell);
}

void freeMemoryBoard(int **board, int **initBoard, int **lastBoard) {
    for (int y = 0; y < HEIGHT; y++) {
        free(board[y]);
        free(initBoard[y]);
        free(lastBoard[y]);
    }
    free(board);
    free(initBoard);
    free(lastBoard);
}

int searchNeighborsBesideCell(int **board, int Y, int X) {
    int count = 0;
    for (int q = -1; q < 2; q++) {
        for (int w = -1; w < 2; w++) {
            if (!(q == 0 && w == 0)) {
                int auxY = Y + q, auxX = X + w;
                auxY < 0 ? auxY = 24 : auxY > 24 ? auxY = 0 : 1;
                auxX < 0 ? auxX = 79 : auxX > 79 ? auxX = 0 : 1;
                if (board[auxY][auxX] == 1) {
                    count += 1;
                }
            }
        }
    }
    return count;
}

int checkGameBoard(int **board, int **clone) {
    int countMatches = 0;
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (clone[y][x] == board[y][x]) {
                countMatches += 1;
            }
        }
    }
    return countMatches;
}

int isWin(int **board, int **lastBoard, int speed, int generation) {
    int result = 1;
    if (checkGameBoard(board, lastBoard) == (WIDTH * HEIGHT)) {
        renderingGameField(board, speed, generation);
        printw("   THE END. Stable configuration!");
        result = 0;
    // } else if (checkGameBoard(board, initBoard) == (WIDTH * HEIGHT)) {
    //     printf("THE END. Periodic configuration!");
    //     result = 0;
    } else if (!isEmptyGameBoard(board)) {
        renderingGameField(board, speed, generation);
        printw("   THE END. No one is alive! Press F. . . ");
        result = 0;
    }
    if (result == 0) {
        timeout(10000);
        getch();
        endwin();
    }
    return result;
}

int **createGameBoard(int **board) {
    for (int y = 0; y < HEIGHT; y++) {
        board[y] = malloc(WIDTH * sizeof(int));
        for (int x = 0; x < WIDTH; x++) {
            board[y][x] = -1;
        }
    }
    return board;
}

int isEmptyGameBoard(int **board) {
    int countMatches = 0;
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (board[y][x] != -1) {
                countMatches += 1;
            }
        }
    }
    return countMatches;
}

int correctScanf(int *number) {
    char bin;
    int result = 0;
    int err_1 = scanf("%d", number);
    int err_2 = scanf("%c", &bin);
    if (err_1) {
        if (err_2 && (bin == ' ' || bin == '\n')) {
            result = err_2;
        }
    }
    return result;
}

int inputData(int **board) {
    int countCell = 0, Y = 0, X = 0, result = 0;
    if (correctScanf(&countCell)) {
        if (countCell >= 0 && countCell <= WIDTH * HEIGHT) {
            for (int q = 0; q < countCell; q++) {
                if (correctScanf(&Y) && Y > 0 && Y <= HEIGHT) {
                    if (correctScanf(&X) && X > 0 && X <= WIDTH) {
                        board[Y - 1][X - 1] = 1;
                        q == (countCell - 1) ? (result = 1) : (result = 0);
                        continue;
                    }
                }
                break;
            }
        }
    }
    return result;
}
