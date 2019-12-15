#include "client.h"

void Wydiaw(int **array, Coord **move, Info *vii) /// Even You Do, I Always Win
{
	int numMove = 0, team = vii->team;
	Coord *actuel = NULL;

	if(*move != NULL) free(*move);
	detection(array, &actuel, vii, team, &numMove);

    if(actuel == NULL) puts("No next move");
	else printf("=> next move (%d:%d)\n", actuel->x, actuel->y);
	*move = actuel;
}

int main(int argc, char *argv[])
{
	int sock = init("Duncan"), temp = 0, i, j, rematch = 0;
	int **array = NULL, binSize = 0, length = 0;
	char *serverMsg = NULL, *binary = NULL, listen = 1;
	Coord *move = NULL, old;
	Info *vii = malloc(sizeof(Info));

	while(1){
		temp = readData(sock, &binary, &binSize, &vii); /// Read turn
		puts("ici");
		if(temp == NOK_MSG) break;
        else if(listen && (temp == OK_MSG || temp == NEXT_TURN)) temp = readData(sock, &binary, &binSize, &vii);
		puts("after");
		makeArray(&array, binary, binSize, vii->size_x, vii->size_y); /// Create / Update local array
        display(array, vii, NULL); /// Display the local array
		if(move != NULL) old.x = move->x, old.y = move->y;
		//Wydiaw(array, &move, vii); /// AI
		minimax(array, &move, vii);
		if(move != NULL && old.x == move->x && old.y == move->y) free(move), move = NULL;
		if(move == NULL || arrayIsFull(array, vii)) move = NULL;
		if(move == NULL) listen = 0; // No confirmation
		else listen = 1;
		printf("Listen is equal to %d\n", listen);
		generateResponse(move, &serverMsg, &length); /// Generate response
		sendData(sock, serverMsg, length); /// Send new move
        if(move == NULL && arrayIsFull(array, vii)){
			puts("end match");
			listen = 1;
			if(rematch) {
				readData(sock, &binary, &binSize, &vii);
				break;
			}
			else rematch++;
		}
	}

	close(sock);
	puts("[+] Disconnected");
	return 0;
}
