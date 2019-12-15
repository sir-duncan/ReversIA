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
void display(int **array, Info *vii);
int heuristic(int **array, Info *vii);
void freeArray(int ***array, Info **vii);
void createCoord(Coord **possible, int y, int x);
void detection(int **array, Coord **possible, Info *vii);
void pathTesting(int **array, Coord **possible, Info *vii, int dir, int y, int x);

void copyBoard(int ***tempArray, int **array, Info *vii)
{
    int i = 0, j = 0;
    for(i = 0; i < vii->size_y; i++){
        for(j = 0; j < vii->size_y; j++){
            (*tempArray)[i][j] == array[i][j];
        }
    }
}

void simulate(int **tempArray, Coord *move, Info *vii)
{
    tempArray[move->y][move->x] = vii->team; /// Plus les autres transformations
}

void Wydiaw(int **array, Coord **move, Info *vii) /// Even You Do, I Always Win
{
	Coord *actuel = NULL; /// Working on actuel, but we can work directly on move
	if(*move != NULL) freeAllMove(move);
    actuel = NULL, *move = NULL;

	detection(array, &actuel, vii);
    vii->team = -1;

    if(actuel == NULL) puts("No next move");
	else printf("=> next move (%d:%d)\n", actuel->x, actuel->y);
	*move = actuel;
}

void minimax(int **array, Info *vii, char team)
{
    int enemy = ((vii->team == 1) ? 2 : 1), i = 0;
    Coord *actuel = NULL;
    detection(array, &actuel, vii);
    if(actuel == NULL) puts("No move to do !");
    else {
        int bestMoveVal = -99;
        Coord *bestMove = NULL;

        int **tempArray = malloc(sizeof(int*) * vii->size_y);
        for(i = 0; i < vii->size_y; i++) tempArray[i] = malloc(sizeof(int) * vii->size_x);
        copyBoard(&tempArray, array, vii);
        simulate(&tempArray, actuel, vii);
        display(array, vii);
        display(tempArray, vii);

/*        while(actuel != 0) {

}*/
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

    display(array, vii); /// Display the local array
    minimax(array, vii, vii->team); /// AI
    printf("heuristic = %d, %d\n", heuristic(array, vii), vii->team);
    freeArray(&array, &vii);
    freeAllMove(&move);
    free(vii);
    puts("\n=> The end");
    return 0;
}

void display(int **array, Info *vii)
{
    int i = 0, j = 0;
    for(i = 0; i < vii->size_y; i++){
        for(j = 0; j < vii->size_x; j++){
            if(array[i][j] == 0) printf(" . "); /// Displaying board
            else printf(" %d ", array[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void createCoord(Coord **possible, int y, int x)
{
    Coord *actuel = malloc(sizeof(Coord));
    actuel->x = x, actuel->y = y;
    actuel->next = *possible;
    *possible = actuel;
}

void pathTesting(int **array, Coord **possible, Info *vii, int dir, int y, int x)
{
    while(array[y][x] == ((vii->team == 1) ? 2 : 1)){
        if(dir == 0) x--, y--;
        else if(dir == 1) y--;
        else if(dir == 2) y--, x++;
        else if(dir == 3) x--;
        else if(dir == 5) x++;
        else if(dir == 6) y++, x--;
        else if(dir == 7) y++;
        else if(dir == 8) y++, x++;
        if(y < 0 || y > vii->size_y - 1 || x < 0 || x > vii->size_x - 1) return;
    }
    if(array[y][x] == 0) createCoord(possible, y, x);
}

void detection(int **array, Coord **possible, Info *vii)
{
    int i = 0, j = 0, enemi = ((vii->team == 1) ? 2 : 1), temp = 0;
    if(array == NULL) puts("Array is NULL");
    for(i = 0; i < vii->size_y; i++){ /// See through the array
        for(j = 0; j < vii->size_x; j++){
            if(array[i][j] == vii->team){ /// For every team token in a game
                for(temp = 0; temp < 9; temp++){ /// For every direction
                    if(i - 1 + (temp / 3) < 0 || j - 1 + (temp % 3) < 0) continue; /// If 0, skip
                    if(i - 1 + (temp / 3) >= vii->size_y || j - 1 + (temp % 3) >= vii->size_x) continue; /// Skip if player
                    if(array[i - 1 + (temp / 3)][j - 1 + (temp % 3)] == enemi)
                        pathTesting(array, possible, vii, temp, i - 1 + (temp / 3), j - 1 + (temp % 3));
                }
            }
        }
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
    printf("score %d\nescore %d\n", score, escore);
    return score - escore;
}
