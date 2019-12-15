#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#ifdef WIN32 // Libraries for Windows
    #include <windows.h>
    #include <winsock2.h>
#else // Libraries for Linux
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #define SOCKET_ERROR -1
    #define INVALID_SOCKET -1
    typedef int SOCKET;
    typedef struct in_addr IN_ADDR;
    #define closesocket(s) close(s)
    typedef struct sockaddr SOCKADDR;
    typedef struct sockaddr_in SOCKADDR_IN;
#endif

#define IP_ADDRESS "192.168.1.22"// "127.0.0.1"
#define PORT 8888
#define SYNCRO 0x55
#define OK_MSG 0x01
#define NOK_MSG 0x00
#define NEW_MOVE 0x03
#define END_MOVE 0x04
#define OKNOK_MSG 0x02
#define NEXT_TURN 0x05
#define PLAYER_OK 0x10
#define CONNECT_MESSAGE 0x01

typedef struct coord {
    int x, y;
    struct coord *next;
} Coord;

typedef struct {
	int size_x, size_y;
	int team;
} Info;

int init(char *pseudo);
int readData(int sock, char **binary, int *binSize, Info **vii);

char *toBinary(char *response, int length, int debut, int *binSize);

void display(int **array, Info *vii);
void sendData(int sock, char *msg, int length);
void createCoord(Coord **possible, int y, int x);
void Wydiaw(int **array, Coord **move, Info *vii);
void detection(int **array, Coord **possible, Info *vii);
void generateResponse(Coord *move, char **serverMsg, int *length);
void makeArray(int ***array, char *binary, int binSize, int size_x, int size_y);
void pathTesting(int **array, Coord **possible, Info *vii, int dir, int y, int x);
