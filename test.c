/**
 * @author Dheovani Xavier da Cruz
 */
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#define ARROW_UP 72
#define ARROW_LEFT 75
#define ARROW_RIGHT 77
#define ARROW_DOWN 80

typedef int bool;

// Player variables
struct node {
    int x, y, dir, maxSize;
    struct node* node;
    char dir;
};

// Game variables
struct food {
    int x, y;
};

int cColumns, cRows;
bool EXIT = FALSE;

/**
 * Returns pressed key
 */
int get_pressed_key() {
    // First value will be '224' to represent an arrow button
    getch();
    _sleep(100);
    return getch();
}

/**
 * Initializes the variables os the game
 */
void set_console_size() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    cColumns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    cRows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

/**
 * Check if collision happened with 1 of the snake's nodes
 */
bool check_collision_with_nodes(struct node* node, int x, int y) {
    if (node->node && check_collision_with_nodes(node->node, x, y)) {
        return TRUE;
    }

    return node->x == x && node->y == y;
}

/**
 * Generate location for food in the map
 */
void set_food_in_map(struct food* food) {
    srand(time(NULL));

    food->x = rand() % cColumns + 10;
    food->y = rand() % cRows + 10;
}

/**
 * After every movement, we check collisions and :
 * 
 * -> End the game if the user hit something
 * 
 * -> Update the snake if the user ate the food
 */
void deal_with_collision(struct node* snake, struct food* food) {
    // Collided with console's limits
    if (snake->x == cColumns || snake->y == cRows || check_collision_with_nodes(snake, snake->x, snake->y)) {
        EXIT = TRUE;
        return;
    }

    if (snake->x == food->x && snake->y == food->y) {
        set_food_in_map(food);
        
        struct node newNode;
        newNode.x = snake->dir == 'x' ? snake->x - 1 : snake->x;
        newNode.y = snake->dir == 'y' ? snake->y - 1 : snake->y;

        snake->node = &newNode;
        return;
    }
}

/**
 * Moves the snake in the map
 */
void move_snake(struct node* snake, int key) {
    switch (key) {
        case ARROW_UP:
            snake->y += 1;
            break;

        case ARROW_DOWN:
            snake->y -= 1;
            break;

        case ARROW_LEFT:
            snake->x -= 1;
            break;

        case ARROW_RIGHT:
            snake->x += 1;
            break;
    }
}

// Método para construir a tela

// Método para iniciar o jogo

int main() {
    return 0;
}