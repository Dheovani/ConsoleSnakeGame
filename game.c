/**
 * @author Dheovani Xavier da Cruz
 */
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <math.h>

#define ARROW_UP 72
#define ARROW_LEFT 75
#define ARROW_RIGHT 77
#define ARROW_DOWN 80

typedef int bool;

// Player variables
struct node {
    int x, y, size;
    struct node* node;
    char dir;
};

// Game variables
struct food {
    int x, y;
};

int cColumns, cRows, maxSize;
bool GAME_OVER = FALSE;

/**
 * Returns pressed key
 * @return int
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
    maxSize = cColumns * cRows;
}

/**
 * Check if collision happened with 1 of the snake's nodes
 * @param struct node* node
 * @param int x
 * @param int y
 * @return bool
 */
bool check_collision_with_nodes(struct node* node, int x, int y) {
    if (node->node && check_collision_with_nodes(node->node, x, y)) {
        return TRUE;
    }

    return node->x == x && node->y == y;
}

/**
 * Generates a random number given a limit
 * @param int limit
 * @return int
 */
int get_random_number(int limit) {
    srand(time(NULL));

    int num = 0;
    while (num <= 0 || num >= limit) {
        num = 1 + (rand() % (limit - 1));
    }

    return num;
}

/**
 * Generate location for food in the map
 * @param struct food* food
 */
void set_food_in_map(struct food* food) {
    if (!cRows && !cColumns) {
        set_console_size();
    }

    food->x = get_random_number(cColumns);
    food->y = get_random_number(cRows);

    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hout == INVALID_HANDLE_VALUE) {
        exit(EXIT_FAILURE);
    }

    DWORD dwWritten = 0;
    COORD cursor = {food->x, food->y};
    const char cs[] = "o";
    WriteConsoleOutputCharacter(hout, cs, strlen(cs), cursor, &dwWritten);
}

/**
 * Draw n the map
 * @param int x
 * @param int y
 * @param char symbol[]
 */
void draw_in_map(int x, int y, char symbol[]) {
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hout == INVALID_HANDLE_VALUE) {
        exit(EXIT_FAILURE);
    }

    DWORD dwWritten = 0;
    COORD cursor = {x, y};
    WriteConsoleOutputCharacter(hout, symbol, strlen(symbol), cursor, &dwWritten);
}

/**
 * Will use this method to always update the last node of the snake
 * @param struct node* snake
 * @param struct node* node
 */
void update_last_node_value(struct node* snake, struct node* node) {
    if (snake->node) {
        update_last_node_value(snake->node, node);
        return;
    }

    snake->node = node;
}

/**
 * After every movement, we check collisions and :
 * -> End the game if the user hit something
 * -> Update the snake if the user ate the food
 * 
 * @param struct node* snake
 * @param struct food* food
 */
void deal_with_collision(struct node* snake, struct food* food) {
    if (snake->y == cRows || !snake->y) {
        draw_in_map(snake->x, snake->y, " ");

        if (!snake->y) {
            snake->y = cRows;
        } else {
            snake->y = 0;
        }

        return;
    }

    if (snake->x == cColumns || !snake->x) {
        draw_in_map(snake->x, snake->y, " ");

        if (!snake->x) {
            snake->x = cColumns;
        } else {
            snake->x = 0;
        }

        return;
    }

    if (snake->node) {
        // -1 because head can't collide with itself
        if (check_collision_with_nodes(snake->node, snake->x, snake->y)) {
            GAME_OVER = TRUE;
            return;
        }
    }

    // Found food
    if (snake->x == food->x && snake->y == food->y) {
        struct node node;
        struct node* node_pt = malloc(sizeof(node));

        node_pt->x = snake->dir == 'x' ? snake->x - 1 : snake->x;
        node_pt->y = snake->dir == 'y' ? snake->y - 1 : snake->y;
        node_pt->node = NULL;

        snake->size = snake->size + 1;
        update_last_node_value(snake, node_pt);
        set_food_in_map(food);
        
        return;
    }
}

/**
 * Update the position of the childrens of head-node when head-node moves
 * @param struct node* snake
 * @param int x
 * @param int y
 */
void update_snake_node_position(struct node* snake, int x, int y) {
    int oldX = snake->x, oldY = snake->y;

    snake->x = x;
    snake->y = y;

    // Draw body
    draw_in_map(snake->x, snake->y, "@");

    if (snake->node) {
        update_snake_node_position(snake->node, oldX, oldY);
    } else {
        // Clean old node position
        draw_in_map(oldX, oldY, " ");
    }
}

/**
 * Moves the snake in the map
 * @param struct node* snake
 * @param int key
 */
void move_snake(struct node* snake, int key) {
    int x = snake->x, y = snake->y;

    switch (key) {
        case ARROW_UP:
            snake->y = snake->y - 1;
            snake->dir = 'y';
            break;

        case ARROW_DOWN:
            snake->y = snake->y + 1;
            snake->dir = 'y';
            break;

        case ARROW_LEFT:
            snake->x = snake->x - 1;
            snake->dir = 'x';
            break;

        case ARROW_RIGHT:
            snake->x = snake->x + 1;
            snake->dir = 'x';
            break;
    }

    // Draw head
    draw_in_map(snake->x, snake->y, "O");

    // Update other node's positions
    if (snake->node) {
        update_snake_node_position(snake->node, x, y);
    } else {
        // Clean old head position
        draw_in_map(x, y, " ");
    }
}

/**
 * Game over!
 * @param struct node* snake
 */
void end_game(struct node* snake) {
    // Clean console
    system("cls");

    if (snake->size == maxSize) {
        printf("Congratulations! You win!\n");
    } else {
        printf("You lose!\n");
    }
}

/**
 * Initialize game
 */
void game() {
    struct food food;
    struct node snake;

    struct node* snake_pt = malloc(sizeof(snake));
    snake_pt->x = 5;
    snake_pt->y = 5;
    snake_pt->size = 1;
    snake_pt->node = NULL;

    struct food* food_pt = malloc(sizeof(food));

    set_console_size();
    set_food_in_map(food_pt);
    draw_in_map(snake_pt->x, snake_pt->y, "O");

    int key = ARROW_RIGHT;
    while (!GAME_OVER) {
        // Change direction
        if (_kbhit()) {
            key = get_pressed_key();
        }

        move_snake(snake_pt, key);
        deal_with_collision(snake_pt, food_pt);
        _sleep(50);
    }

    end_game(snake_pt);
}

int main() {
    game();
    return 0;
}
