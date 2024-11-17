//SOURCE CODE - TESTING

// Libraries

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>
#include <windows.h>
#include <time.h>

// Colors

#define LIGHT_GRAY "\x1b[38;5;145m"
#define BLUE "\x1b[38;5;20m"
#define GREEN "\x1b[38;5;28m"
#define RED "\x1b[38;5;196m"
#define PURPLE "\x1b[38;5;19m"
#define DARK_RED "\x1b[38;5;88m"
#define CYAN "\x1b[38;5;23m"
#define NORMAL "\x1b[30m"
#define GRAY "\x1b[38;5;8m"

// Booleans

#define True 1
#define False 0

// Symbolic Constants

#define MAP_COLUMNS 20
#define MAP_ROWS 20
#define CHECK_RANGE 1
#define MAX_LIVES 3
#define MAX_MINE 10
#define MAX_FLAG 100
#define HEART_ICON 003
#define FLAG_ICON 16
#define MINE_ICON 15
#define DETONATED 9
#define MAX_MOVE 15

// This is the codename for mines in map_data

#define MINE 10

// Globals

HANDLE consoleHandle;
CONSOLE_CURSOR_INFO cursor;
COORD c;


const char ctarget_row[MAX_MOVE] = {'A','B','B','A','L','A','J','C','E','D','D','L','K','C','E'};
const char ctarget_col[MAX_MOVE] = {'A','D','F','H','R','A','S','C','D','C','E','K','J','D','E'};

const int mine_distribution[MAX_MINE][2] = {{0,0},{1,7},{2,2},{3,3},{4,4},{10,17},{16,26},{8,19},{20,1},{12,17}};

const int right_padding = (MAP_COLUMNS*2)+8;
const char *color[9] = {LIGHT_GRAY, BLUE, GREEN, RED, PURPLE, DARK_RED, CYAN, NORMAL, GRAY};

char map_visual[MAP_ROWS][MAP_COLUMNS];
int map_data[MAP_ROWS][MAP_COLUMNS];
int map_color[MAP_ROWS][MAP_COLUMNS];
int map_flag[MAP_ROWS][MAP_COLUMNS];
int is_revealed[MAP_ROWS][MAP_COLUMNS];
int lives = MAX_LIVES;
int mines = MAX_MINE;
int flag_count = MAX_FLAG;
int moves_finished = 0;

int target_row[MAX_MOVE] = {0};
int target_col[MAX_MOVE] = {0};

// Prototypes

void game_environment(void);
void initialize_map(void);
void interact_prompt(void);
void place_hints(void);
void place_mines(void);
void select_position(void);
int count_adjacent_mines(int row, int col);
void show_hints(int row, int col);
int check_win(void);

void show_interact_options(void);
void game_over_dialog(void);
void gotoxy(int x, int y);
void show_map(int mode);
void hide_cursor(void);
void show_cursor(void);
void win_dialog(void);
void show_lives(void);
void title(void);



// Main


int main()
{
    game_environment();
}



// Functions

void game_environment(void)
{
    // Initialization phase

    initialize_map();
    place_mines();
    place_hints();

    // Display

    system("COLOR F0");
    interact_prompt();
}

void interact_prompt(void)
{
    system("cls");
    hide_cursor();
    title();
    show_map(0);
    show_interact_options();

    select_position();
}

void initialize_map(void)
{
    int i, j;

    for (i = 0; i < MAP_ROWS; i++)
        {
        for (j = 0; j < MAP_COLUMNS; j++)
        {
            map_flag[i][j] = 0;
            map_data[i][j] = 0;
            map_color[i][j] = 7;
            map_visual[i][j] = 254;
            is_revealed[i][j] = 0;
        }
    }
}

void place_hints(void)
{
    int row, col, adjacent_mines;

    for (row = 0; row < MAP_ROWS; row++)
    {
        for (col = 0; col < MAP_COLUMNS; col++)
        {
            if (map_data[row][col] != MINE) // skip the tiles with mines
            {
                adjacent_mines = count_adjacent_mines(row, col);
                map_data[row][col] = adjacent_mines;
            }
        }
    }
}

int count_adjacent_mines(int row, int col)
{
    int i, j, has_mine, mine_count = 0;

    for (i = row - CHECK_RANGE; i <= row + CHECK_RANGE; i++)
    {
        for (j = col - CHECK_RANGE; j <= col + CHECK_RANGE; j++)
        {
            has_mine = (map_data[i][j] == MINE);

            if (i >= 0 && i < MAP_ROWS && j >= 0 && j < MAP_COLUMNS && has_mine)
            {
                mine_count++;
            }
        }
    }

    return mine_count;
}


void select_position(void)
{
    int has_won = 0;

    if (lives == 0)
    {
        getch();
        game_over_dialog();
    }
    if (has_won == True)
            win_dialog();

    target_row[moves_finished] = ctarget_row[moves_finished] - 65;
    target_col[moves_finished] = ctarget_col[moves_finished] - 65;

    if (map_data[target_row[moves_finished]][target_col[moves_finished]] == MINE) // if a mine is found
    {
        lives--;
        mines--;
        is_revealed[target_row[moves_finished]][target_col[moves_finished]] = 1;
        map_data[target_row[moves_finished]][target_col[moves_finished]] = DETONATED; //remove the mine once hit
        map_color[target_row[moves_finished]][target_col[moves_finished]] = 7; //color code for black
        map_visual[target_row[moves_finished]][target_col[moves_finished]] = MINE_ICON;
    }
    else
    {
        show_hints(target_row[moves_finished], target_col[moves_finished]);
    }

    has_won = check_win();

    // Add 1 to flag

    flag_count += map_flag[target_row[moves_finished]][target_col[moves_finished]] == 1;
    map_flag[target_row[moves_finished]][target_col[moves_finished]] = 0;

    getch();
    moves_finished++;
    interact_prompt();
}

int check_win(void)
{
    int hidden_mines = 0, hidden_tiles = 0; // number of 0s in is_reveal array
    int i, j;

    for (i = 0; i < MAP_ROWS; i++)
    {
        for (j = 0; j < MAP_COLUMNS; j++)
        {
            hidden_tiles += is_revealed[i][j] == 0;
            hidden_mines += is_revealed[i][j] == 0 && map_data[i][j] == MINE;
        }
    }

    if (hidden_tiles == hidden_mines)
    {
        return 1;
    }

    return 0;
}

void place_mines(void)
{
    for (int i = 0; i < MAX_MINE; i++)
    {
        map_data[mine_distribution[i][0]][mine_distribution[i][1]] = MINE;
    }
}

void title(void)
{
  printf(
    "\n  Submitted by Team C | Activity 2 | Computer Programming 2 | BSIS - 1AB\n"
    "  ------------------------------------------------------------------------\n"
    "                                  * MINESWEEPER *                           \n"
    "  ------------------------------------------------------------------------\n");
}

void hide_cursor(void)
{
    consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    cursor.dwSize = 100;
    cursor.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &cursor);
}

void show_cursor(void)
{
    consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    cursor.dwSize = 100;
    cursor.bVisible = TRUE;
    SetConsoleCursorInfo(consoleHandle, &cursor);
}

void gotoxy(int x, int y)
{
    c.X = x;
    c.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),c);
}

void show_lives(void)
{
    gotoxy(right_padding, 6);
    printf("Lives: %s", RED);

    for (int i = 0; i < lives; i++)
    {
        printf("%c ", HEART_ICON);
    }

    printf("%s", NORMAL);
    return;
}

void show_hints(int row, int col)
{
    if (row < 0 || row >= MAP_ROWS || col < 0 || col >= MAP_COLUMNS || is_revealed[row][col] == 1)
    {
        return;
    }

    if (map_data[row][col] == 0)
    {
        flag_count += map_flag[row][col] == 1;
        map_flag[row][col] = 0;
        map_color[row][col] = 0;
        is_revealed[row][col] = 1;

        show_hints(row+1, col);
        show_hints(row-1, col);
        show_hints(row, col+1);
        show_hints(row, col-1);
    }

    if (map_data[row][col] != 0 && map_data[row][col] != DETONATED) // if the data is a number other than 0 and 9 (9 which means a bomb has been found), show the numbers
    {
        map_color[row][col] = map_data[row][col];
        map_visual[row][col] = map_data[row][col] + 48;
        is_revealed[row][col] = 1;
    }
}


void show_map(int mode) // mode 1 - reveals the bomb to the user, mode 0 - default
{
    printf("\n    ");

    for (int j = 0; j < MAP_COLUMNS; j++)
    {
        printf("%c ", j + 'A');
    }

    printf("\n");

    for (int i = 0; i < MAP_ROWS; i++)
    {
        printf("  %c", i + 'A');

        for (int j = 0; j < MAP_COLUMNS; j++)
        {
            if (map_data[i][j] == MINE && mode == 1)
            {
                map_visual[i][j] = MINE_ICON;
                printf(" %s%c%s", color[map_flag[i][j] == 1 ? 3 : map_color[i][j]], map_visual[i][j], NORMAL);
            }
            else
                printf(" %s%c%s", color[map_flag[i][j] == 1 ? 3 : map_color[i][j]], map_flag[i][j] == 1 ? FLAG_ICON : map_visual[i][j], NORMAL);
        }

        printf("\n"); // Newline for next row
    }

    return;
}

void show_interact_options(void)
{
    show_lives();

    gotoxy(right_padding, 7);
    printf("Flags: %s%i%s", BLUE, flag_count, NORMAL);

    gotoxy(right_padding, 8);
    printf("Mines Remaining: %s%i%s", BLUE, mines, NORMAL);

    gotoxy(right_padding, 10);
    printf("Turn: %s%i%s", BLUE, moves_finished+1, NORMAL);

    gotoxy(right_padding, 12);
    printf("Row Input: %s%c%s", BLUE, ctarget_row[moves_finished], NORMAL);

    gotoxy(right_padding, 13);
    printf("Column Input: %s%c%s", BLUE, ctarget_col[moves_finished], NORMAL);

    return;
}

void game_over_dialog(void)
{
    hide_cursor();
    system("cls");
    title();
    show_map(1);

    gotoxy(right_padding, 7);
    printf("DEAR PLAYER, YOU RAN OUT OF LIVES!");

    gotoxy(right_padding, 8);
    printf("PRESS ANY KEY TO EXIT");

    getch();
    exit(1);
}

void win_dialog(void)
{
    hide_cursor();
    system("cls");
    title();
    show_map(0);

    gotoxy(right_padding, 7);
    printf("MINEFIELD CLEARED SUCCESSFULLY, CONGRATULATIONS!");

    gotoxy(right_padding, 8);
    printf("PRESS ANY KEY TO EXIT");

    getch();
    exit(1);
}
