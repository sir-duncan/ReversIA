#include "client.h"

int main(int argc, char *argv[])
{
	int sock = init("Wydiaw"), temp = 0, i, j, rematch = 0;
	int **array = NULL, binSize = 0, length = 0;
	char *serverMsg = NULL, *binary = NULL, listen = 1;
	Coord *move = NULL, old;
	Info *vii = malloc(sizeof(Info));

	while(1){
		//system("cls");
		temp = readData(sock, &binary, &binSize, &vii); /// Read turn
		if(temp == NOK_MSG) break;
        else if(listen && (temp == OK_MSG || temp == NEXT_TURN)) temp = readData(sock, &binary, &binSize, &vii);
		makeArray(&array, binary, binSize, vii->size_x, vii->size_y); /// Create / Update local array
		if(move != NULL) old.x = move->x, old.y = move->y;
		printf("\n");
		display(array, vii, move); /// Display the local array
		Wydiaw(array, &move, vii);
		if(move != NULL && old.x == move->x && old.y == move->y) free(move), move = NULL;
		if(move == NULL || arrayIsFull(array, vii)) move = NULL;
		if(move == NULL) listen = 0; // No confirmation
		else listen = 1;
		if(move != NULL){
			if((move->y < 2 || move->y > vii->size_y - 3) && (move->x < 2 || move->x > vii->size_x - 3)){
				if((move->y == 0 || move->y == vii->size_y - 1) && (move->x == 0 || move->x == vii->size_x - 1)) puts("Angle !");
				else puts("Buffer !");
			}
			else if(((move->y > 0 || move->y < vii->size_y - 1) && (move->x == 0 || move->x == vii->size_x - 1))
				  || ((move->y == 0 || move->y == vii->size_y - 1) && (move->x > 0 || move->x < vii->size_x - 1))) puts("Border !");
		}
		generateResponse(move, &serverMsg, &length); /// Generate response
		sendData(sock, serverMsg, length); /// Send new move
        if(move == NULL && arrayIsFull(array, vii)){
			readData(sock, &binary, &binSize, &vii);
			puts("[+] The match is over");
			temp = victory(array, vii);
			if(temp >= (vii->size_x * vii->size_y) / 2) printf("[+] We won ! (%d pieces)\n", temp);
			else printf("[-] We lost ... (%d pieces)\n", temp);
			listen = 1;
			if(rematch) break;
			else rematch++;
		}
	}

	close(sock);
	puts("[+] Disconnected");
	return 0;
}
