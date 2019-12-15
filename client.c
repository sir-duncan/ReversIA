#include "client.h"

int readData(int sock, char **binary, int *binSize, Info **vii)
{
	int i = 0, j = 0, taille, tempSize = 0, **array = NULL;
	char response[300] = {0};
	if(recv(sock, response, sizeof(char) * 24, 0) < 0)
        printf("[-] Error while reading in the socket");
	else{
		taille = response[1];
        if(taille == 20) taille += 4;
		printf("[+] Server Message (%d) : ", taille);
		for(i = 0; i < (taille > strlen(response) ? taille : 5); i++) printf(" %d ", response[i]);
		printf("\n");

		if(response[0] != SYNCRO) printf("[-] Message not for us : %d\n", response[0]);
		else if(response[2] == PLAYER_OK){ /// Color of team
			if(response[3] == 1) puts("[+] We are team Black (1)");
			else if(response[3] == 2) puts("[+] We are team White (2)");
			(*vii)->team = response[3];
		}
		else if(response[2] == NEXT_TURN){
			puts("[+] Our turn to play");
			(*vii)->size_x = response[5], (*vii)->size_y = response[6];
			if(response[3] == -1) puts("[-] There was no previous move");
			else printf("[+] Previous move was : [ %d ; %d ]\n", response[3], response[4]);
			*binary = toBinary(response, taille, 7, binSize);
		}
		else if(response[2] == OKNOK_MSG){
			if(response[3] == OK_MSG){
				puts("[+] New move accepted");
				return OK_MSG;//readData(sock, board, vii);
			}
			else if(response[3] == NOK_MSG){
				puts("[-] New move denied");
                return NOK_MSG; /// NEW_MOVE not accepted
             }
		}
	}
    return NEXT_TURN;
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
                for(temp = 0; temp < 9; temp++){
                    if(i - 1 + (temp / 3) < 0 || j - 1 + (temp % 3) < 0) continue;
                    if(i - 1 + (temp / 3) >= vii->size_y || j - 1 + (temp % 3) >= vii->size_x) continue;
                    if(array[i - 1 + (temp / 3)][j - 1 + (temp % 3)] == enemi)
                        pathTesting(array, possible, vii, temp, i - 1 + (temp / 3), j - 1 + (temp % 3));
                }
            }
        }
    }
}

void makeArray(int ***array, char *binary, int binSize, int size_x, int size_y)
{
    int i = 0, j = 0, temp = 0;
    if(binary == NULL) puts("Binary is NULL");
    if(*array == NULL){
        *array = (int**)malloc(sizeof(int *) * size_y); /// Initialization of the array
        for(i = 0; i < size_y; i++) (*array)[i] = malloc(sizeof(int) * size_x);
    }
    for(i = 0; i < size_y; i++){
        for(j = 0; j < size_x; j++){
            temp = (j * 2) + (i * 2 * size_x);
            if(temp + 1 >= binSize){ /// Defensive coding +
                (*array)[i][j] = 0;/// Adaptation 1D array to 2D array
                continue;
            }
            if(binary[temp] == 0 && binary[temp + 1] == 1) (*array)[i][j] = 1;
            else if(binary[temp] == 1 && binary[temp + 1] == 0) (*array)[i][j] = 2;
            else (*array)[i][j] = 0;
        }
    }
}

char *toBinary(char *response, int length, int debut, int *binSize)
{
    char *bin = NULL, *tempArray = NULL, octet[50];
    int i = 0, taille = 0, tempSize = 0, tempVar = 0, n, b = 8;
    bin = malloc(sizeof(char) * ((length - debut) * b)); /// Preparing Binary
    for(i = debut; i < length; i++, tempSize = 0){
        for(tempVar = 0; tempVar < 50; tempVar++) octet[tempVar] = 0; // Reinitialize array
        if(response[i] < 0) n = 256 + response[i]; /// if negative number
        else n = response[i];
        while(n != 0) { /// Decimal to binary convertion
            octet[b - tempSize - 1] = n % 2;
            n /= 2, tempSize++;
        }
        if(tempSize < b) tempSize = b;
        for(tempVar = 0; tempVar < tempSize; tempVar++, taille++) bin[taille] = octet[tempVar];
    }
    *binSize = taille;
    return bin;
}

int arrayIsFull(int **array, Info *vii)
{
    int i = 0, j = 0;
    for(i = 0; i < vii->size_y; i++){
        for(j = 0; j < vii->size_y; j++)
            if(array[i][j] == 0) return 0;
    }
    return 1;
}

void sendData(int sock, char *msg, int length)
{
    int i = 0;
    if(msg == NULL) return;
    printf("[*] Sending : ");
    for(i < 0; i < length; i++) printf(" %d ", msg[i]);
    printf("\n");
    if(send(sock, msg, length, 0) < 0)
        puts("[-] Error : When sending new message");
}

void generateResponse(Coord *move, char **serverMsg, int *length)
{
    if(*serverMsg != NULL) free(*serverMsg);
    *serverMsg = malloc(sizeof(6));
    (*serverMsg)[0] = SYNCRO, (*serverMsg)[1] = 2, (*serverMsg)[2] = NEW_MOVE;
    if(move != NULL)
        (*serverMsg)[3] = move->x, (*serverMsg)[4] = move->y, (*serverMsg)[5] = NEW_MOVE + move->x + move->y;
    else (*serverMsg)[3] = -1, (*serverMsg)[4] = -1, (*serverMsg)[5] = 1;
    (*serverMsg)[6] = '\0', *length = 6;
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
}

int init(char *pseudo)
{
    SOCKET sock;
    SOCKADDR_IN server;

#ifdef WIN32
    WSADATA wsa;
    if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("[-] Failed to initialise Winsock. Error Code : %d",WSAGetLastError());
        return 1;
    }
#endif

	int CRC = 0, i = 0;
	unsigned char bytes[4];
	char *server_reply = NULL, serverMsg[256];

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1) puts("[-] Could not create socket");
	else puts("[+] Socket created");

	server.sin_addr.s_addr = inet_addr(IP_ADDRESS); // 192.168.0.104  127.0.0.1
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);

	puts("[*] Connecting...");
    if(connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0){
		puts("[-] Connection failed. Exiting !");
		exit(EXIT_FAILURE);
	}
    bytes[0] = server.sin_addr.s_addr & 0xFF;
    bytes[1] = (server.sin_addr.s_addr >> 8) & 0xFF;
    bytes[2] = (server.sin_addr.s_addr >> 16) & 0xFF;
    bytes[3] = (server.sin_addr.s_addr >> 24) & 0xFF;
	printf("[+] Connected to server %d.%d.%d.%d on port %d\n", bytes[0], bytes[1], bytes[2], bytes[3], ntohs(server.sin_port));

	serverMsg[0] = SYNCRO, serverMsg[1] = strlen(pseudo) + 1;
	serverMsg[2] = CONNECT_MESSAGE, serverMsg[3] = strlen(pseudo);
	for(i = 0; i < strlen(pseudo); i++) serverMsg[i + 4] = pseudo[i];
	for(i = 2; i < strlen(pseudo) + 4; i++) CRC += serverMsg[i];
	serverMsg[strlen(pseudo) + 4] = CRC;
	sendData(sock, serverMsg, strlen(serverMsg)); /// Sending the connect message
	return sock;
}
