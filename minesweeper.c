
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define SIZE 6
#define MINES 8

typedef struct {
    int isMine;
    int isRevealed;
    int adjacentMines;
} Cell;

typedef struct {
    Cell board[SIZE][SIZE];
    int minesRemaining;
    int cellsRevealed;
} Game;

void initializeBoard(Game *game) {
    // Initialize all cells
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            game->board[i][j].isMine = 0;
            game->board[i][j].isRevealed = 0;
            game->board[i][j].adjacentMines = 0;
        }
    }
    
    // Place mines randomly
    int minesPlaced = 0;
    srand(time(NULL));
    while (minesPlaced < MINES) {
        int row = rand() % SIZE;
        int col = rand() % SIZE;
        if (!game->board[row][col].isMine) {
            game->board[row][col].isMine = 1;
            minesPlaced++;
        }
    }
    
    // Calculate adjacent mines for each cell
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (!game->board[i][j].isMine) {
                int count = 0;
                for (int di = -1; di <= 1; di++) {
                    for (int dj = -1; dj <= 1; dj++) {
                        int ni = i + di;
                        int nj = j + dj;
                        if (ni >= 0 && ni < SIZE && nj >= 0 && nj < SIZE) {
                            if (game->board[ni][nj].isMine) {
                                count++;
                            }
                        }
                    }
                }
                game->board[i][j].adjacentMines = count;
            }
        }
    }
    
    game->minesRemaining = MINES;
    game->cellsRevealed = 0;
}

void displayBoard(Game *game, int showMines) {
    printf("\n   ");
    for (int j = 0; j < SIZE; j++) {
        printf("%d ", j);
    }
    printf("\n   ");
    for (int j = 0; j < SIZE * 2 - 1; j++) {
        printf("-");
    }
    printf("\n");
    
    for (int i = 0; i < SIZE; i++) {
        printf("%d| ", i);
        for (int j = 0; j < SIZE; j++) {
            if (!game->board[i][j].isRevealed) {
                printf("? ");
            } else if (showMines && game->board[i][j].isMine) {
                printf("* ");
            } else if (game->board[i][j].isMine) {
                printf("X ");
            } else if (game->board[i][j].adjacentMines == 0) {
                printf("  ");
            } else {
                printf("%d ", game->board[i][j].adjacentMines);
            }
        }
        printf("\n");
    }
}

void floodFill(Game *game, int row, int col) {
    if (row < 0 || row >= SIZE || col < 0 || col >= SIZE) {
        return;
    }
    
    if (game->board[row][col].isRevealed) {
        return;
    }
    
    game->board[row][col].isRevealed = 1;
    game->cellsRevealed++;
    
    if (game->board[row][col].adjacentMines == 0) {
        for (int di = -1; di <= 1; di++) {
            for (int dj = -1; dj <= 1; dj++) {
                floodFill(game, row + di, col + dj);
            }
        }
    }
}

int revealCell(Game *game, int row, int col) {
    if (row < 0 || row >= SIZE || col < 0 || col >= SIZE) {
        printf("Invalid coordinates!\n");
        return 0;
    }
    
    if (game->board[row][col].isRevealed) {
        printf("Cell already revealed!\n");
        return 0;
    }
    
    if (game->board[row][col].isMine) {
        printf("You hit a mine! Game Over!\n");
        return -1;
    }
    
    floodFill(game, row, col);
    return 1;
}

int checkWin(Game *game) {
    return game->cellsRevealed == (SIZE * SIZE - MINES);
}

void playGame() {
    Game game;
    int gameOver = 0;
    int won = 0;
    
    printf("Welcome to Minesweeper (6x6)!\n");
    printf("Mines: %d\n", MINES);
    printf("Instructions: Enter row and column (0-5) to reveal a cell\n\n");
    
    initializeBoard(&game);
    displayBoard(&game, 0);
    
    while (!gameOver && !won) {
        int row, col;
        printf("\nEnter row (0-5): ");
        scanf("%d", &row);
        printf("Enter column (0-5): ");
        scanf("%d", &col);
        
        int result = revealCell(&game, row, col);
        
        if (result == -1) {
            gameOver = 1;
            printf("\nFinal board:\n");
            displayBoard(&game, 1);
            printf("You revealed %d cells before hitting a mine.\n", game.cellsRevealed);
        } else if (result == 1) {
            displayBoard(&game, 0);
            if (checkWin(&game)) {
                won = 1;
                printf("\nCongratulations! You won!\n");
                printf("You revealed all %d safe cells!\n", game.cellsRevealed);
            }
        }
    }
}

int main() {
    playGame();
    return 0;
}