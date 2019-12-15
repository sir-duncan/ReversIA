#include "client.h"

void Wydiaw(int **array, Coord **move, Info *vii) /// Even You Do, I Always Win
{
	Coord *actuel = NULL;

	if(*move != NULL) free(*move);
	detection(array, &actuel, vii);

    if(actuel == NULL) puts("No next move");
	else printf("=> next move (%d:%d)\n", actuel->x, actuel->y);
	*move = actuel;
}

int main(int argc, char *argv[])
{
	int sock = init("Duncan"), temp = 0, i, j;
	int **array = NULL, binSize = 0, length = 0;
	char *serverMsg = NULL, *binary = NULL, listen = 1;
	Coord *move = NULL;
	Info *vii = malloc(sizeof(Info));

	while(1){
		temp = readData(sock, &binary, &binSize, &vii); /// Read turn
        if(temp == NOK_MSG) break;
        else if(listen && (temp == OK_MSG || temp == NEXT_TURN)) temp = readData(sock, &binary, &binSize, &vii);
        makeArray(&array, binary, binSize, vii->size_x, vii->size_y); /// Create / Update local array
        display(array, vii); /// Display the local array
		Wydiaw(array, &move, vii); /// AI
        if(move == NULL && !arrayIsFull(array, vii)) listen = 0; // No confirmation
        else listen = 1;
		generateResponse(move, &serverMsg, &length); /// Generate response
		usleep(300000); /// Small delay, otherwise the server doesn't keep up
		sendData(sock, serverMsg, length); /// Send new move
        if(move == NULL && arrayIsFull(array, vii)) readData(sock, &binary, &binSize, &vii);
	}

	close(sock);
	puts("[+] Disconnected");
	return 0;
}
