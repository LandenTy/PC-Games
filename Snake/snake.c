#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <string.h>

#define WIDTH 40
#define HEIGHT 20
#define MAX_SNAKE_LENGTH 100
#define HIGH_SCORE_FILE "highscore.txt"

enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };
enum MenuOption { MENU_PLAY = 0, MENU_HIGH_SCORE = 1, MENU_QUIT = 2 };

typedef struct {
    int x, y;
} Point;

Point snake[MAX_SNAKE_LENGTH];
int snakeLength = 5;
enum Direction dir = RIGHT;
int gameOver = 0;
int score = 0;

Point fruit;

// --- Utility ---
void gotoxy(int x, int y) {
    COORD pos = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void hideCursor() {
    CONSOLE_CURSOR_INFO ci = {1, FALSE};
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ci);
}

void setColor(int c) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

// --- High Score Handling ---
void loadHighScore(int *highScore, char *highScoreName) {
    FILE *file = fopen(HIGH_SCORE_FILE, "r");
    if (file) {
        fscanf(file, "%d %3s", highScore, highScoreName); // Read high score and player name
        fclose(file);
    } else {
        *highScore = 0; // Default high score if file doesn't exist
        strcpy(highScoreName, "???");
    }
}

void saveHighScore(int highScore, const char *name) {
    FILE *file = fopen(HIGH_SCORE_FILE, "w");
    if (file) {
        fprintf(file, "%d %s", highScore, name);
        fclose(file);
    }
}

// --- Drawing ---
void drawBorder() {
    setColor(15);
    for (int i = 0; i <= WIDTH; i++) {
        gotoxy(i, 0); printf("#");
        gotoxy(i, HEIGHT); printf("#");
    }
    for (int i = 0; i <= HEIGHT; i++) {
        gotoxy(0, i); printf("#");
        gotoxy(WIDTH, i); printf("#");
    }
}

void spawnFruit() {
    int valid = 0;
    while (!valid) {
        fruit.x = (rand() % (WIDTH - 2)) + 1;
        fruit.y = (rand() % (HEIGHT - 2)) + 1;
        valid = 1;
        for (int i = 0; i < snakeLength; i++) {
            if (snake[i].x == fruit.x && snake[i].y == fruit.y) {
                valid = 0;
                break;
            }
        }
    }
}

// --- Input ---
void input() {
    if (_kbhit()) {
        switch (_getch()) {
            case 72: if (dir != DOWN) dir = UP; break;
            case 80: if (dir != UP) dir = DOWN; break;
            case 75: if (dir != RIGHT) dir = LEFT; break;
            case 77: if (dir != LEFT) dir = RIGHT; break;
            case 'x': gameOver = 1; break;
        }
    }
}

// --- Game Logic ---
void logic() {
    Point prevTail = snake[snakeLength - 1];

    // Move body
    for (int i = snakeLength - 1; i > 0; i--)
        snake[i] = snake[i - 1];

    // Move head
    switch (dir) {
        case LEFT:  snake[0].x--; break;
        case RIGHT: snake[0].x++; break;
        case UP:    snake[0].y--; break;
        case DOWN:  snake[0].y++; break;
    }

    // Wall or self collision
    if (snake[0].x == 0 || snake[0].x == WIDTH || snake[0].y == 0 || snake[0].y == HEIGHT)
        gameOver = 1;
    for (int i = 1; i < snakeLength; i++)
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y)
            gameOver = 1;

    // Eating fruit
    if (snake[0].x == fruit.x && snake[0].y == fruit.y) {
        if (snakeLength < MAX_SNAKE_LENGTH) {
            snake[snakeLength] = prevTail; // grow at tail
            snakeLength++;
        }
        score += 10;
        spawnFruit();
    } else {
        // Erase tail if not growing
        gotoxy(prevTail.x, prevTail.y);
        printf(" ");
    }
}

void draw() {
    gotoxy(0, HEIGHT + 1);
    setColor(15);
    printf("Score: %d", score);

    // Fruit
    gotoxy(fruit.x, fruit.y);
    setColor(12);
    printf("F");

    // Snake
    for (int i = 0; i < snakeLength; i++) {
        gotoxy(snake[i].x, snake[i].y);
        if (i == 0) { setColor(10); printf("^"); }
        else if (i == snakeLength - 1) { setColor(7); printf("O"); }
        else { setColor(11); printf("0"); }
    }
}

// --- Main Menu ---
int runMenu() {
    int selection = 0;
    int running = 1;
    while (running) {
        system("cls");

        for (int i = 0; i < 3; i++) {
            int y = 5 + i * 6;
            int selected = (selection == i);

            setColor(15);
            gotoxy(8, y);     printf("########################################");
            gotoxy(8, y + 1); setColor(selected ? 14 : 8); printf("#~-~=~-~=~-~=~-~=-~=-~=~-~=~-~=~-~=~-~#");

            gotoxy(8, y + 2); setColor(selected ? 14 : 8);
            if (i == 0)
                printf("#~-~=~-~=~-~=~-~=  PLAY  ~-~=~-~=~-~=~-~#");
            else if (i == 1)
                printf("#~-~=~-~=~-~=~-~=  HIGH SCORE  ~-~=~-~=~-~#");
            else
                printf("#~-~=~-~=~-~=~-~=  QUIT  ~-~=~-~=~-~=~-~#");

            gotoxy(8, y + 3); setColor(selected ? 14 : 8); printf("#~-~=~-~=~-~=~-~=-~=-~=~-~=~-~=~-~=~-~#");
            setColor(15);
            gotoxy(8, y + 4); printf("########################################");
        }

        int ch = _getch();
        if (ch == 72) selection = (selection == 0) ? 2 : selection - 1;
        else if (ch == 80) selection = (selection == 2) ? 0 : selection + 1;
        else if (ch == 13) running = 0; // ENTER
    }
    return selection;
}

// --- High Score Menu ---
void runHighScoreMenu() {
    int highScore = 0;
    char highScoreName[4];
    loadHighScore(&highScore, highScoreName);

    int running = 1;
    while (running) {
        system("cls");
        gotoxy(10, 5); setColor(15); printf("High Score");
        gotoxy(10, 7); setColor(15); printf("Score: %d", highScore);
        gotoxy(10, 8); setColor(15); printf("Player: %s", highScoreName);
        gotoxy(10, 10); setColor(15); printf("Press any key to return to the main menu...");
        _getch();
        running = 0; // Exit the loop and return to the main menu
    }
}

// --- Game Over ---
void handleGameOver() {
    int highScore = 0;
    char highScoreName[4];
    loadHighScore(&highScore, highScoreName);

    int newHighScore = 0;

    if (score > highScore) {
        system("cls");
        gotoxy(10, HEIGHT / 2); setColor(14);
        printf("NEW HIGH SCORE! Enter your name (max 3 characters): ");

        while (_kbhit()) _getch();  // Clear buffer

        char playerName[4];
        int i = 0;
        while (i < 3) {
            char ch = _getch();
            if (ch == 13) break;  // Enter
            if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) {
                playerName[i++] = ch;
                printf("%c", ch);
            }
        }
        playerName[i] = '\0';

        saveHighScore(score, playerName);
        newHighScore = 1;
        Sleep(1000);
    }

    // Game Over Menu
    int selection = 0;
    int running = 1;
    while (running) {
        system("cls");

        gotoxy(14, 3); setColor(12); printf("GAME OVER");
        gotoxy(12, 4); setColor(7); printf("Your final score was: %d", score);
        if (newHighScore) {
            gotoxy(10, 5); setColor(10); printf(">>> New High Score! <<<");
        }

        for (int i = 0; i < 2; i++) {
            int y = 8 + i * 6;
            int selected = (selection == i);

            setColor(15);
            gotoxy(8, y);     printf("########################################");
            gotoxy(8, y + 1); setColor(selected ? 14 : 8); printf("#~-~=~-~=~-~=~-~=-~=-~=~-~=~-~=~-~=~-~#");

            gotoxy(8, y + 2); setColor(selected ? 14 : 8);
            if (i == 0)
                printf("#~-~=~-~=~-~= PLAY AGAIN ~-~=~-~=~-~=~-~#");
            else
                printf("#~-~=~-~=~-~=~~  QUIT  ~~=-~=-~=~-~=~-~#");

            gotoxy(8, y + 3); setColor(selected ? 14 : 8); printf("#~-~=~-~=~-~=~-~=-~=-~=~-~=~-~=~-~=~-~#");
            setColor(15);
            gotoxy(8, y + 4); printf("########################################");
        }

        int ch = _getch();
        if (ch == 72) selection = (selection == 0) ? 1 : 0;
        else if (ch == 80) selection = (selection == 1) ? 0 : 1;
        else if (ch == 13) running = 0; // ENTER
    }

    if (selection == 0) {
        main(); // Play again
    } else {
        exit(0); // Quit
    }
}

// --- Main ---
int main() {
    hideCursor();
    srand(GetTickCount());

    int menuChoice = runMenu();
    while (menuChoice != MENU_QUIT) {
        if (menuChoice == MENU_HIGH_SCORE) {
            runHighScoreMenu();
        } else if (menuChoice == MENU_PLAY) {
            system("cls");

            snakeLength = 5;
            dir = RIGHT;
            score = 0;
            gameOver = 0;

            for (int i = 0; i < snakeLength; i++) {
                snake[i].x = 10 - i;
                snake[i].y = 10;
            }

            drawBorder();
            spawnFruit();

            while (!gameOver) {
                input();
                logic();
                draw();
                Sleep(100);
            }

            handleGameOver();
        }

        menuChoice = runMenu();  // After each game or high score menu, go back to main menu
    }

    return 0;
}
