
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
    int minesHit;
    int lives;
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
    game->minesHit = 0;
    game->lives = 1;
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

int askCPythonQuestion() {
    // Array of C and Python logic questions
    const char *questions[] = {
        "In C, what does 'sizeof(int)' typically return? (A) 2  (B) 4  (C) 8  (D) 16",
        "In Python, what will print(True + True) output? (A) 1  (B) 2  (C) True  (D) Error",
        "In C, what is the output of printf(\"%d\", 5 / 2)? (A) 2.5  (B) 2  (C) 3  (D) 2.0",
        "In Python, what is type([]) in Python? (A) tuple  (B) list  (C) array  (D) dict",
        "In C, which operator has the highest precedence? (A) +  (B) *  (C) []  (D) -",
        "In Python, what does 'not True and False' evaluate to? (A) True  (B) False  (C) Error  (D) None"
    };
    
    const char *answers[] = {
        "B", "B", "B", "B", "C", "B"
    };
    
    int questionNum = rand() % 6;
    char userAnswer[10];
    
    printf("\n%s\n", questions[questionNum]);
    printf("Your answer: ");
    scanf("%s", userAnswer);
    
    // Convert to uppercase for comparison
    for (int i = 0; userAnswer[i]; i++) {
        if (userAnswer[i] >= 'a' && userAnswer[i] <= 'z') {
            userAnswer[i] = userAnswer[i] - 32;
        }
    }
    
    if (strcmp(userAnswer, answers[questionNum]) == 0) {
        return 1;
    }
    
    printf("Wrong answer! Correct answer was: %s\n", answers[questionNum]);
    return 0;
}

void displayRoastingMessage() {
    const char *roasts[] = {
        "\n🔥 OH NO! Second mine hit! 🔥",
        "That's it! You just triggered the 'double trouble' achievement!",
        "Congratulations, you've achieved: 'Minesweeper Champion of Explosions'!",
        "Your mining career just reached an all-time low. Literally. BOOM!",
        "Two mines? Really? Maybe try a different hobby... like Tic-Tac-Toe?",
        "Game Over! You just proved that second chances aren't always a blessing!",
        "You hit TWO mines. Even the luck gods have abandoned you now.",
        "BOOM! BOOM! You're speedrunning failure like a pro!",
        "Two strikes and you're out! This isn't baseball, buddy. 💢",
        "You're like a 'smart bomb' - smart at finding EVERY single mine!"
    };
    
    int roastNum = rand() % 10;
    printf("%s\n", roasts[roastNum]);
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
        game->minesHit++;
        
        if (game->minesHit == 1) {
            printf("\n💥 MINE HIT! 💥\n");
            printf("Don't worry! You get ONE lifeline. Answer this question correctly to continue!\n");
            
            if (askCPythonQuestion()) {
                printf("\n🎉 CORRECT! You've earned a SECOND LIFE! 🎉\n");
                printf("Lives remaining: %d\n", ++game->lives);
                floodFill(game, row, col);
                return 1;
            } else {
                printf("\nGame Over! Wrong answer!\n");
                return -1;
            }
        } else if (game->minesHit >= 2) {
            game->lives--;
            displayRoastingMessage();
            printf("Lives remaining: %d\n", game->lives);
            return -1;
        }
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
    printf("🛡️  Starting Lives: 1 (Answer the logic question correctly to gain a second life!)\n");
    printf("Instructions: Enter row and column (0-5) to reveal a cell\n\n");
    
    initializeBoard(&game);
    displayBoard(&game, 0);
    
    while (!gameOver && !won) {
        int row, col;
        printf("\n[Lives: %d] ", game.lives);
        printf("Enter row (0-5): ");
        scanf("%d", &row);
        printf("Enter column (0-5): ");
        scanf("%d", &col);
        
        int result = revealCell(&game, row, col);
        
        if (result == -1) {
            gameOver = 1;
            printf("\nFinal board:\n");
            displayBoard(&game, 1);
            printf("You revealed %d cells before the game ended.\n", game.cellsRevealed);
        } else if (result == 1) {
            displayBoard(&game, 0);
            if (checkWin(&game)) {
                won = 1;
                printf("\n🎉 Congratulations! You won! 🎉\n");
                printf("You revealed all %d safe cells without exhausting all lives!\n", game.cellsRevealed);
            }
        }
    }
}

int main() {
    playGame();
    return 0;
}