#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

typedef struct coord {
    int x, y;
    struct coord *next;
} Coord;

typedef struct {
	int size_x, size_y;
	char team;
} Info;

void freeAllMove(Coord **move);
int heuristic(int **array, Info *vii);
void freeArray(int ***array, Info **vii);
void createCoord(Coord **possible, int y, int x);
void display(int **array, Info *vii, Coord *move);
void copyBoard(int ***tempArray, int **array, Info *vii);
void simulate(int ***tempArray, Coord *move, Info *vii, char team);
void detection(int **array, Coord **possible, Info *vii, char team, int *numMove);
void pathTesting(int **array, Info *vii, Coord **test, int dir, char team);
void changeLine(int ***array, Info *vii, Coord *debut, int dir, char team);

/*{
	Coord *actuel = NULL; /// Working on actuel, but we can work directly on move
	if(*move != NULL) freeAllMove(move);
    actuel = NULL, *move = NULL;

	detection(array, &actuel, vii, vii->team);
    vii->team = -1;

    if(actuel == NULL) puts("No next move");
	else printf("=> next move (%d:%d)\n", actuel->x, actuel->y);
	*move = actuel;
}*/

int minimaxValue(int **array, Info *vii, char player, char team, char depth)
{
    //printf("Depth: %d\nTeam: %d\n", depth, team);
    if(depth == 0) return heuristic(array, vii); /// If depth is deep enough
    int numMove = 0, count = 1;
    char enemy = ((team == 1) ? 2 : 1), i = 0;
    Coord *actuel = NULL;
    detection(array, &actuel, vii, team, &numMove);
    if(actuel == NULL) puts("No move to do !");
    else {
        int bestMoveVal = ((player == team) ? -9999 : 9999), val = 0;
        Coord *bestMove = NULL;
        int **tempArray = malloc(sizeof(int*) * vii->size_y);
        for(i = 0; i < vii->size_y; i++) tempArray[i] = malloc(sizeof(int) * vii->size_x); /// Do the free thin'
        while(actuel != 0) {
            //printf("Trying next move %d / %d\n", count, numMove);
            copyBoard(&tempArray, array, vii); /// Fresh copy
            simulate(&tempArray, actuel, vii, team); /// Simulate move
            //display(tempArray, vii, actuel);
            val = minimaxValue(tempArray, vii, player, enemy, depth - 1);
            if(player == team && val > bestMoveVal) bestMoveVal = val, bestMove = actuel;
            else if(player != team && val < bestMoveVal) bestMoveVal = val, bestMove = actuel;
            actuel = actuel->next, count++;
        }
        return bestMoveVal;
    }
    return -1; /// Should never pass through here
}

void minimax(int **array, Coord **move, Info *vii)
{
    int numMove = 0, count = 1;
    char team = vii->team, enemy = ((team == 1) ? 2 : 1), i = 0;
    Coord *actuel = NULL;
    detection(array, &actuel, vii, team, &numMove);
    if(actuel == NULL) puts("No move to do !");
    else {
        int bestMoveVal = -9999, val = 0;
        Coord *bestMove = NULL;
        int **tempArray = malloc(sizeof(int*) * vii->size_y);
        for(i = 0; i < vii->size_y; i++) tempArray[i] = malloc(sizeof(int) * vii->size_x);
        while(actuel != NULL) {
            //printf("Trying next move (base) %d / %d\n", count, numMove);
            copyBoard(&tempArray, array, vii); /// Fresh copy
            simulate(&tempArray, actuel, vii, team); /// Simulate move
            //display(tempArray, vii, actuel);
            val = minimaxValue(tempArray, vii, team, enemy, 3);
            if(val > bestMoveVal) bestMoveVal = val, bestMove = actuel;
            actuel = actuel->next, count++;
        }
        *move = bestMove;
    }
}

int main()
{
    int i = 0, j = 0;
    Info *vii = malloc(sizeof(Info));
    vii->size_x = 8, vii->size_y = 8, vii->team = 1;
    Coord *move = NULL;
    int **array = malloc(sizeof(int*) * vii->size_y);
    for(i = 0; i < vii->size_y; i++) array[i] = malloc(sizeof(int) * vii->size_x);

    int board[8][8] = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 0},
        {0, 2, 1, 0, 0, 0, 0, 0},
        {0, 0, 2, 1, 1, 1, 0, 2},
        {0, 0, 1, 2, 2, 0, 2, 2},
        {0, 1, 0, 1, 2, 2, 0, 2},
        {0, 0, 0, 1, 2, 2, 2, 2},
        {0, 0, 1, 1, 1, 1, 1, 1}
    };

    for(i = 0; i < vii->size_y; i++) /// Copy the board
        for(j = 0; j < vii->size_x; j++) array[i][j] = board[i][j];

    display(array, vii, NULL); /// Display the local array
    //Wydiaw(array, &move, vii);
    minimax(array, &move, vii); /// AI
    printf("best move : [ %d, %d ]\n", move->x, move->y);
    printf("heuristic = %d, %d\n", heuristic(array, vii), vii->team);
    freeArray(&array, &vii);
    freeAllMove(&move);
    free(vii);
    puts("\n=> The end");
    return 0;
}

void display(int **array, Info *vii, Coord *move)
{
    int i = 0, j = 0;
    for(i = 0; i < vii->size_y; i++){
        for(j = 0; j < vii->size_x; j++){
            if(array[i][j] == 0) printf(" . "); /// Displaying board
            else if(move != NULL && move->y == i && move->x == j) printf("-%d-", array[i][j]);
            else if(array[i][j] == 2) printf(" O ");
            else printf(" %d ", array[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void copyBoard(int ***tempArray, int **array, Info *vii)
{
    int i = 0, j = 0;
    for(i = 0; i < vii->size_y; i++){
        for(j = 0; j < vii->size_y; j++){
            (*tempArray)[i][j] = array[i][j];
        }
    }
}

void simulate(int ***tempArray, Coord *move, Info *vii, char team)
{
    int temp = 0, i = move->y, j = move->x, enemy = ((team == 1) ? 2 : 1);
    Coord *test = malloc(sizeof(Coord));
    test->y = move->y, test->x = move->x;
    (*tempArray)[i][j] = team;
    for(temp = 0; temp < 9; temp++){ /// For every direction
        if(i - 1 + (temp / 3) < 0 || j - 1 + (temp % 3) < 0) continue; /// If 0, skip
        if(i - 1 + (temp / 3) >= vii->size_y || j - 1 + (temp % 3) >= vii->size_x) continue; /// Skip if player
        if((*tempArray)[i - 1 + (temp / 3)][j - 1 + (temp % 3)] == enemy){
            test->y = move->y, test->x = move->x;
            pathTesting(*tempArray, vii, &test, temp, team);
            if((*tempArray)[test->y][test->x] == team) changeLine(tempArray, vii, move, temp, team);
        }
    }
}

void createCoord(Coord **possible, int y, int x)
{
    Coord *actuel = malloc(sizeof(Coord));
    actuel->x = x, actuel->y = y;
    actuel->next = *possible;
    *possible = actuel;
}

void pathTesting(int **array, Info *vii, Coord **test, int dir, char team)
{
    int x = (*test)->x, y = (*test)->y, enemy = ((team == 1) ? 2 : 1);
    do{
        if(dir == 0) x--, y--;
        else if(dir == 1) y--;
        else if(dir == 2) y--, x++;
        else if(dir == 3) x--;
        else if(dir == 5) x++;
        else if(dir == 6) y++, x--;
        else if(dir == 7) y++;
        else if(dir == 8) y++, x++;
        if(y < 0 || y > vii->size_y - 1 || x < 0 || x > vii->size_x - 1) return;
        else (*test)->y = y, (*test)->x = x;
    }while(array[y][x] == enemy);
}

void detection(int **array, Coord **possible, Info *vii, char team, int *numMove)
{
    int i = 0, j = 0, enemy = ((team == 1) ? 2 : 1), temp = 0;
    Coord *test = malloc(sizeof(Coord));
    test->next = NULL;
    if(array == NULL) puts("Array is NULL");
    for(i = 0; i < vii->size_y; i++){ /// See through the array
        for(j = 0; j < vii->size_x; j++){
            if(array[i][j] == team){ /// For every team token in a game
                for(temp = 0; temp < 9; temp++){ /// For every direction
                    if(i - 1 + (temp / 3) < 0 || j - 1 + (temp % 3) < 0) continue; /// If 0, skip
                    if(i - 1 + (temp / 3) >= vii->size_y || j - 1 + (temp % 3) >= vii->size_x) continue; /// Skip if player
                    if(array[i - 1 + (temp / 3)][j - 1 + (temp % 3)] == enemy){
                        test->y = i - 1 + (temp / 3), test->x = j - 1 + (temp % 3);
                        pathTesting(array, vii, &test, temp, team);
                        if(array[test->y][test->x] == 0){
                            *numMove += 1;
                            createCoord(possible, test->y, test->x);
                        }
                    }
                }
            }
        }
    }
}

void changeLine(int ***array, Info *vii, Coord *debut, int dir, char team)
{
    int x = debut->x, y = debut->y, enemy = ((team == 1) ? 2 : 1);
    while(1){
        if(dir == 0) x--, y--;
        else if(dir == 1) y--;
        else if(dir == 2) y--, x++;
        else if(dir == 3) x--;
        else if(dir == 5) x++;
        else if(dir == 6) y++, x--;
        else if(dir == 7) y++;
        else if(dir == 8) y++, x++;
        if(y < 0 || y > vii->size_y - 1 || x < 0 || x > vii->size_x - 1) return;
        if((*array)[y][x] == enemy) (*array)[y][x] = team;
        else break;
    }
}

void freeArray(int ***array, Info **vii)
{
    int i = 0;
    for(i = 0; i < (*vii)->size_y; i++) free((*array)[i]);
}

void freeAllMove(Coord **move)
{
    if(*move == NULL) return;
    Coord *actuel = NULL, *save = *move;
    while(save != NULL){
        actuel = save;
        save = save->next;
        free(actuel);
    }
}

int heuristic(int **array, Info *vii) /// Get the heuristic value of the board
{
    int enemy = ((vii->team == 1) ? 2 : 1), i = 0, j = 0, score = 0, escore = 0;
    for(i = 0; i < vii->size_y; i++){
        for(j = 0; j < vii->size_x; j++){
            if(array[i][j] == vii->team) score++;
            else if(array[i][j] == enemy) escore++;
        }
    }
    //printf("score %d\nescore %d\n", score, escore);
    return score - escore;
}
