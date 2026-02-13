
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <raylib.h>

#define SIZE 6
#define MINES 8
#define CELL_SIZE 60
#define PADDING 20
#define WINDOW_WIDTH 520
#define WINDOW_HEIGHT 720

typedef struct {
    int isMine;
    int isRevealed;
    int isFlagged;
    int adjacentMines;
} Cell;

typedef struct {
    Cell board[SIZE][SIZE];
    int minesRemaining;
    int cellsRevealed;
    int gameOver;
    int won;
    int lost;
} Game;

typedef struct {
    Rectangle rect;
    const char *text;
    Color color;
    Color hoverColor;
    int isHovered;
} Button;

Game game;
Button resetButton;
Button quitButton;

void initializeBoard(Game *game) {
    // Initialize all cells
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            game->board[i][j].isMine = 0;
            game->board[i][j].isRevealed = 0;
            game->board[i][j].isFlagged = 0;
            game->board[i][j].adjacentMines = 0;
        }
    }
    
    // Place mines randomly
    int minesPlaced = 0;
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
    game->gameOver = 0;
    game->won = 0;
    game->lost = 0;
}

void initializeButtons() {
    resetButton.rect = (Rectangle){PADDING, SIZE * CELL_SIZE + PADDING + 40, 100, 40};
    resetButton.text = "RESET";
    resetButton.color = GREEN;
    resetButton.hoverColor = LIME;
    resetButton.isHovered = 0;
    
    quitButton.rect = (Rectangle){WINDOW_WIDTH - PADDING - 100, SIZE * CELL_SIZE + PADDING + 40, 100, 40};
    quitButton.text = "QUIT";
    quitButton.color = RED;
    quitButton.hoverColor = MAROON;
    quitButton.isHovered = 0;
}

int isMouseOverButton(Button *button) {
    return CheckCollisionPointRec(GetMousePosition(), button->rect);
}

void drawButton(Button *button) {
    button->isHovered = isMouseOverButton(button);
    
    Color currentColor = button->isHovered ? button->hoverColor : button->color;
    DrawRectangleRec(button->rect, currentColor);
    DrawRectangleLinesEx(button->rect, 2, BLACK);
    
    int textWidth = MeasureText(button->text, 20);
    int textX = button->rect.x + (button->rect.width - textWidth) / 2;
    int textY = button->rect.y + (button->rect.height - 20) / 2;
    DrawText(button->text, textX, textY, 20, BLACK);
}

void drawCell(int row, int col, int x, int y) {
    Cell *cell = &game.board[row][col];
    Color cellColor = LIGHTGRAY;
    Color borderColor = DARKGRAY;
    
    // Draw cell background
    DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, cellColor);
    DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, borderColor);
    
    if (cell->isRevealed) {
        if (cell->isMine) {
            // Draw mine
            DrawCircle(x + CELL_SIZE / 2, y + CELL_SIZE / 2, 15, RED);
            DrawCircle(x + CELL_SIZE / 2, y + CELL_SIZE / 2, 12, DARKRED);
        } else if (cell->adjacentMines > 0) {
            // Draw number
            char numStr[2];
            sprintf(numStr, "%d", cell->adjacentMines);
            int textWidth = MeasureText(numStr, 30);
            DrawText(numStr, x + CELL_SIZE / 2 - textWidth / 2, y + CELL_SIZE / 2 - 15, 30, BLUE);
        } else {
            // Empty cell (0 adjacent mines)
            DrawRectangle(x + 2, y + 2, CELL_SIZE - 4, CELL_SIZE - 4, WHITE);
        }
    } else {
        // Unrevealed cell
        DrawRectangleGradientV(x + 2, y + 2, CELL_SIZE - 4, CELL_SIZE - 4, LIGHTGRAY, GRAY);
        
        if (cell->isFlagged) {
            // Draw flag
            DrawTriangle((Vector2){x + CELL_SIZE / 2 + 5, y + 10},
                        (Vector2){x + CELL_SIZE / 2 + 5, y + 25},
                        (Vector2){x + CELL_SIZE / 2 + 20, y + 17}, YELLOW);
        } else {
            DrawText("?", x + CELL_SIZE / 2 - 10, y + CELL_SIZE / 2 - 15, 30, DARKGRAY);
        }
    }
}

void drawBoard() {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            int x = PADDING + j * CELL_SIZE;
            int y = PADDING + i * CELL_SIZE;
            drawCell(i, j, x, y);
        }
    }
}

void floodFill(int row, int col) {
    if (row < 0 || row >= SIZE || col < 0 || col >= SIZE) {
        return;
    }
    
    if (game.board[row][col].isRevealed) {
        return;
    }
    
    game.board[row][col].isRevealed = 1;
    game.cellsRevealed++;
    
    if (game.board[row][col].adjacentMines == 0) {
        for (int di = -1; di <= 1; di++) {
            for (int dj = -1; dj <= 1; dj++) {
                floodFill(row + di, col + dj);
            }
        }
    }
}

void revealCell(int row, int col) {
    if (row < 0 || row >= SIZE || col < 0 || col >= SIZE) {
        return;
    }
    
    if (game.board[row][col].isRevealed || game.board[row][col].isFlagged) {
        return;
    }
    
    if (game.board[row][col].isMine) {
        game.board[row][col].isRevealed = 1;
        game.lost = 1;
        game.gameOver = 1;
        // Reveal all mines
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (game.board[i][j].isMine) {
                    game.board[i][j].isRevealed = 1;
                }
            }
        }
        return;
    }
    
    floodFill(row, col);
    
    if (game.cellsRevealed == (SIZE * SIZE - MINES)) {
        game.won = 1;
        game.gameOver = 1;
    }
}

void toggleFlag(int row, int col) {
    if (row < 0 || row >= SIZE || col < 0 || col >= SIZE) {
        return;
    }
    
    if (game.board[row][col].isRevealed) {
        return;
    }
    
    game.board[row][col].isFlagged = !game.board[row][col].isFlagged;
    if (game.board[row][col].isFlagged) {
        game.minesRemaining--;
    } else {
        game.minesRemaining++;
    }
}

void handleMouseInput() {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetMousePosition();
        
        // Check reset button
        if (isMouseOverButton(&resetButton)) {
            initializeBoard(&game);
            return;
        }
        
        // Check quit button
        if (isMouseOverButton(&quitButton)) {
            exit(0);
        }
        
        // Check board cells
        if (!game.gameOver) {
            for (int i = 0; i < SIZE; i++) {
                for (int j = 0; j < SIZE; j++) {
                    int x = PADDING + j * CELL_SIZE;
                    int y = PADDING + i * CELL_SIZE;
                    Rectangle cellRect = {x, y, CELL_SIZE, CELL_SIZE};
                    
                    if (CheckCollisionPointRec(mousePos, cellRect)) {
                        revealCell(i, j);
                        return;
                    }
                }
            }
        }
    }
    
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        Vector2 mousePos = GetMousePosition();
        
        // Check board cells
        if (!game.gameOver) {
            for (int i = 0; i < SIZE; i++) {
                for (int j = 0; j < SIZE; j++) {
                    int x = PADDING + j * CELL_SIZE;
                    int y = PADDING + i * CELL_SIZE;
                    Rectangle cellRect = {x, y, CELL_SIZE, CELL_SIZE};
                    
                    if (CheckCollisionPointRec(mousePos, cellRect)) {
                        toggleFlag(i, j);
                        return;
                    }
                }
            }
        }
    }
}

void drawUI() {
    // Draw title
    const char *title = "MINESWEEPER 6x6";
    int titleWidth = MeasureText(title, 40);
    DrawText(title, WINDOW_WIDTH / 2 - titleWidth / 2, 10, 40, DARKBLUE);
    
    // Draw mines remaining
    char minesText[32];
    sprintf(minesText, "Mines: %d", game.minesRemaining);
    DrawText(minesText, PADDING, SIZE * CELL_SIZE + PADDING + 10, 20, BLACK);
    
    // Draw game status
    if (game.gameOver) {
        if (game.won) {
            DrawText("YOU WON!", WINDOW_WIDTH / 2 - 80, SIZE * CELL_SIZE + PADDING + 90, 30, GREEN);
        } else {
            DrawText("GAME OVER!", WINDOW_WIDTH / 2 - 90, SIZE * CELL_SIZE + PADDING + 90, 30, RED);
        }
    } else {
        char cellsText[32];
        sprintf(cellsText, "Revealed: %d", game.cellsRevealed);
        DrawText(cellsText, WINDOW_WIDTH / 2 - 60, SIZE * CELL_SIZE + PADDING + 10, 20, BLACK);
    }
    
    // Draw buttons
    drawButton(&resetButton);
    drawButton(&quitButton);
}

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Minesweeper Game");
    SetTargetFPS(60);
    
    srand(time(NULL));
    initializeBoard(&game);
    initializeButtons();
    
    while (!WindowShouldClose()) {
        handleMouseInput();
        
        BeginDrawing();
        ClearBackground(DARKGRAY);
        
        drawBoard();
        drawUI();
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}