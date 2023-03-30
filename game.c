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
    int x: 5;
    int y: 5;
    struct node* node;
    char dir;
};

// Game variables
struct food {
    int x, y;
};

int cColumns, cRows;
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
        printf("You've hit yourself");
        return TRUE;
    }

    return node->x == x && node->y == y;
}

/**
 * Generate location for food in the map
 * @param struct food* food
 */
void set_food_in_map(struct food* food) {
    if (!cRows && !cColumns) {
        set_console_size();
    }
    srand(time(NULL));

    food->x = rand() % cColumns + 10;
    food->y = rand() % cRows + 10;

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
 * After every movement, we check collisions and :
 * -> End the game if the user hit something
 * -> Update the snake if the user ate the food
 * 
 * @param struct node* snake
 * @param struct food* food
 */
void deal_with_collision(struct node* snake, struct food* food) {
    // Collided with console's limits
    if (snake->x == cColumns || snake->y == cRows || !snake->x || !snake->y) {
        GAME_OVER = TRUE;
        printf("You lose!");
        return;
    }

    // Collided with it's own body
    if (snake->node) {
        int headX = snake->x;
        int headY = snake->y;

        if (check_collision_with_nodes(snake, headX, headY)) {
            GAME_OVER = TRUE;
            printf("You lose!");
            return;
        }
    }

    // Found food
    if (snake->x == food->x && snake->y == food->y) {
        struct node newNode;
        newNode.x = snake->dir == 'x' ? snake->x - 1 : snake->x;
        newNode.y = snake->dir == 'y' ? snake->y - 1 : snake->y;
        newNode.node = NULL;

        snake->node = &newNode;
        set_food_in_map(food);
        
        return;
    }
}

/**
 * Draw snake nodes in the map
 * @param int x
 * @param int y
 * @param char symbol[]
 */
void draw_node(int x, int y, char symbol[]) {
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hout == INVALID_HANDLE_VALUE) {
        exit(EXIT_FAILURE);
    }

    DWORD dwWritten = 0;
    COORD cursor = {x, y};
    WriteConsoleOutputCharacter(hout, symbol, strlen(symbol), cursor, &dwWritten);
}

/**
 * Update the position of the childrens of head-node when head-node moves
 * @param struct node* snake
 * @param int x
 * @param int y
 * @param char dir
 */
void update_snake_node_position(struct node* snake, int x, int y, char dir) {
    int oldX = snake->x, oldY = snake->y;
    char oldDir = snake->dir;

    snake->x = x;
    snake->y = y;
    snake->dir = dir;

    // Draw body
    draw_node(snake->x, snake->y, "@");

    if (snake->node) {
        update_snake_node_position(snake->node, oldX, oldY, oldDir);
    } else {
        // Clean old head position
        draw_node(oldX, oldY, " ");
    }
}

/**
 * Moves the snake in the map
 * @param struct node* snake
 * @param int key
 */
void move_snake(struct node* snake, int key) {
    int x = snake->x, y = snake->y;
    char dir = snake->dir;

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
    draw_node(snake->x, snake->y, "O");

    // Update other node's positions
    if (snake->node) {
        update_snake_node_position(snake->node, x, y, dir);
    } else {
        // Clean old head position
        draw_node(x, y, " ");
    }
}

/**
 * Print game board
 */
void print_board() {
    if (!cRows && !cColumns) {
        set_console_size();
    }

    int x, y;
    for (x = 0; x < cColumns; x ++) {
        printf("X");
    }

    for (y = cRows - 1; y > 2; y --) {
        printf("X");

        for (x = 0; x < cColumns -2; x ++) {
            printf(" ");
        }

        printf("X\n");
    }

    for (x = 0; x < cColumns; x ++) {
        printf("X");
    }
}

/**
 * Initialize game
 */
void game() {
    struct node snake;
    snake.x = 5;
    snake.y = 5;
    snake.node = NULL;
    snake.dir = 'x';

    struct food food;

    struct node* snake_pt = &snake;
    struct food* food_pt = &food;

    // Initialize board
    set_console_size();
    print_board();

    // Generate food
    set_food_in_map(food_pt);

    // Snake head
    draw_node(snake_pt->x, snake_pt->y, "O");

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
}

int main() {
    game();
    return 0;
}
