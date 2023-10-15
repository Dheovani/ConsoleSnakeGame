/**
 * @author Dheovani Xavier da Cruz
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <Windows.h>
#include <math.h>

#define ARROW_UP 72
#define ARROW_LEFT 75
#define ARROW_RIGHT 77
#define ARROW_DOWN 80

// Player variables
typedef struct Node {
    int x, y, size;
    char dir;
    struct Node* node;
} Node;

// Game variables
typedef struct Food {
    int x, y;
} Food;

CONSOLE_SCREEN_BUFFER_INFO csbi;
int cColumns, cRows, maxSize;
bool GAME_OVER = FALSE;

/**
 * Returns pressed key
 * @return int
 */
int get_pressed_key()
{
    // First value will be '224' to represent an arrow button
    getch();
    Sleep(100);
    return getch();
}

/**
 * Initializes the variables os the game
 */
void set_console_size()
{
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hout, &csbi);
    SetConsoleTextAttribute(hout, FOREGROUND_GREEN);

    cColumns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    cRows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    maxSize = cColumns * cRows;
}

/**
 * Check if collision happened with 1 of the snake's nodes or the map's limits
 * @param Node* node
 * @param int x
 * @param int y
 * @return bool
 */
bool collided_with_nodes(Node* node, int x, int y)
{
    if (node->node && collided_with_nodes(node->node, x, y))
        return TRUE;

    return node->x == x && node->y == y;
}

/**
 * Generates a random number given a limit
 * @param int limit
 * @return int
 */
int get_random_number(int limit)
{
    srand(time(NULL));

    int num = 0;
    while (num <= 6 || num >= limit - 6)
        num = 1 + (rand() % (limit - 1));

    return num;
}

/**
 * Generate location for food in the map
 * @param Food* food
 */
void set_food_in_map(Food* food)
{
    if (!cRows && !cColumns)
        set_console_size();

    food->y = get_random_number(cRows);
    food->x = get_random_number(cColumns);

    // X needs to be even
    if (food->x % 2 != 0)
        food->x += 1;

    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);

    if (hout == INVALID_HANDLE_VALUE)
        exit(EXIT_FAILURE);

    DWORD dwWritten = 0;
    COORD cursor = { food->x, food->y };
    WriteConsoleOutputCharacter(hout, "o", (DWORD) strlen("o"), cursor, &dwWritten);
}

/**
 * Draws map limitations
 */
void draw_map_limits()
{
    DWORD dwWritten = 0;
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
    const SMALL_RECT wd = csbi.srWindow;

    if (hout == INVALID_HANDLE_VALUE)
        exit(EXIT_FAILURE);

    for (int c = 5; c < cColumns - 5; c++)
    {
        COORD top = { c, wd.Bottom - 5 };
        WriteConsoleOutputCharacter(hout, "--", (DWORD) strlen("--"), top, &dwWritten);

        COORD bottom = { c, wd.Top + 5 };
        WriteConsoleOutputCharacter(hout, "--", (DWORD) strlen("--"), bottom, &dwWritten);
    }

    for (int r = 5; r < cRows - 5; r++)
    {
        COORD right = { wd.Right - 5, r };
        WriteConsoleOutputCharacter(hout, "|", (DWORD) strlen("|"), right, &dwWritten);

        COORD left = { wd.Left + 6, r };
        WriteConsoleOutputCharacter(hout, "|", (DWORD) strlen("|"), left, &dwWritten);
    }
}

/**
 * Draw n the map
 * @param int x
 * @param int y
 * @param char symbol[]
 */
void draw_in_map(int x, int y, char symbol[])
{
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);

    if (hout == INVALID_HANDLE_VALUE)
        exit(EXIT_FAILURE);

    DWORD dwWritten = 0;
    COORD cursor = {x, y};
    WriteConsoleOutputCharacter(hout, symbol, (DWORD) strlen(symbol), cursor, &dwWritten);
}

char* read_from_map(int x, int y, int length, char* result)
{
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);

    if (hout == INVALID_HANDLE_VALUE)
        exit(EXIT_FAILURE);

    COORD start = { x, y };
    DWORD dwRead;

    if (!ReadConsoleOutputCharacter(hout, result, length, start, &dwRead))
    {
        // Handle errors here
        printf("Error reading console: %d\n", GetLastError());
        exit(EXIT_FAILURE);
    }

    result[length] = '\0';
    return result;
}

/**
 * Will use this method to always update the last node of the snake
 * @param Node* snake
 * @param Node* node
 */
void update_last_node_value(Node* snake, Node* node)
{
    if (snake->node)
    {
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
 * @param Node* snake
 * @param Food* food
 */
void deal_with_collision(Node* snake, Food* food)
{
    const SMALL_RECT w = csbi.srWindow;

    if (snake->y == w.Bottom - 5 || snake->y == w.Top + 5)
    {
        draw_in_map(snake->x, snake->y, "--");
        snake->y = (snake->y == w.Top + 5) ? w.Bottom - 5 : w.Top + 5;

        return;
    }

    if (snake->x == w.Right - 5 || snake->x == w.Left + 6)
    {
        draw_in_map(snake->x, snake->y, "|");
        snake->x = (snake->x == w.Left + 6) ? w.Right - 5 : w.Left + 6;

        return;
    }

    if (snake->node && collided_with_nodes(snake->node, snake->x, snake->y))
    {
        GAME_OVER = TRUE;
        return;
    }

    // Found food
    if (snake->x == food->x && snake->y == food->y)
    {
        Node* node_pt = malloc(sizeof(Node));

        node_pt->x = snake->dir == 'x' ? snake->x - 1 : snake->x;
        node_pt->y = snake->dir == 'y' ? snake->y - 1 : snake->y;
        node_pt->node = NULL;

        snake->size = snake->size + 1;
        update_last_node_value(snake, node_pt);

        if (snake->size == maxSize)
        {
            GAME_OVER = TRUE;
            return;
        }

        set_food_in_map(food);
        
        return;
    }
}

/**
 * Update the position of the childrens of head-node when head-node moves
 * @param Node* snake
 * @param int x
 * @param int y
 */
void update_snake_node_position(Node* snake, int x, int y)
{
    int oldX = snake->x, oldY = snake->y;
    char* value = " ";

    snake->x = x;
    snake->y = y;
    
    if (oldY == csbi.srWindow.Bottom - 5 || oldY == csbi.srWindow.Top + 5)
        value = "--";
    else if (oldX == csbi.srWindow.Right - 5 || oldX == csbi.srWindow.Left + 6)
        value = "|";

    // Draw body
    draw_in_map(snake->x, snake->y, "@");

    if (snake->node)
        update_snake_node_position(snake->node, oldX, oldY);
    else
        draw_in_map(oldX, oldY, value); // Clean old node position
}

/**
 * Moves the snake in the map
 * @param Node* snake
 * @param int key
 */
void move_snake(Node* snake, int key)
{
    int x = snake->x, y = snake->y;
    char* value = " ";

    switch (key)
    {
        case ARROW_UP:
            snake->y = snake->y - 1;
            snake->dir = 'y';
            break;

        case ARROW_DOWN:
            snake->y = snake->y + 1;
            snake->dir = 'y';
            break;

        case ARROW_LEFT:
            snake->x = snake->x - 2;
            snake->dir = 'x';
            break;

        case ARROW_RIGHT:
            snake->x = snake->x + 2;
            snake->dir = 'x';
            break;
    }

    if (y == csbi.srWindow.Bottom - 5 || y == csbi.srWindow.Top + 5)
        value = "--";
    else if (x == csbi.srWindow.Right - 5 || x == csbi.srWindow.Left + 6)
        value = "|";

    // Draw head
    draw_in_map(snake->x, snake->y, "O");

    // Update other node's positions
    if (snake->node)
        update_snake_node_position(snake->node, x, y);
    else
        draw_in_map(x, y, value); // Clean old head position
}

/**
 * Game over!
 * @param Node* snake
 */
void end_game(Node* snake)
{
    // Clean console
    system("cls");

    if (snake->size == maxSize)
        printf("Congratulations! You win!\n");
    else
        printf("You lose!\n");
}

/**
 * Delete all node of the snake
 * @param Node* snake
 */
void delete_nodes(Node* snake)
{
    if (snake->node)
        delete_nodes(snake->node);

    free(snake);
    snake = NULL;
}

/**
 * Initialize game
 */
void game()
{
    set_console_size();
    draw_map_limits();

    Node snake = { .x = 10, .y = 10, .size = 1, .node = NULL };
    Food food = { .x = get_random_number(cColumns), .y = get_random_number(cRows) };

    // Draw head
    draw_in_map(snake.x, snake.y, "O");
    set_food_in_map(&food);

    int key = ARROW_RIGHT;
    while (!GAME_OVER)
    {
        // Change direction
        if (_kbhit())
            key = get_pressed_key();

        move_snake(&snake, key);
        deal_with_collision(&snake, &food);
        Sleep(50);
    }

    end_game(&snake);
    
    if (snake.node)
        delete_nodes(snake.node);
}

int main()
{
    game();
    return 0;
}
