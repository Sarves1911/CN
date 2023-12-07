#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock.h>

#define PORT 8080



int main() {

   WSADATA wsaData;
    SOCKET listenSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    char buffer[128];

    if (WSAStartup(MAKEWORD(1,1), &wsaData) != 0) {
        printf("WSAStartup failed. Error: %d\n", WSAGetLastError());
        return 1;
    }

    listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET) {
        printf("Socket creation failed. Error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Bind failed. Error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    if (listen(listenSocket, 5) == SOCKET_ERROR) {
        printf("Listen failed. Error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    printf("Server is listening for connections...\n");

    clientSocket = accept(listenSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
    if (clientSocket == INVALID_SOCKET) {
        printf("Accept failed. Error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    printf("Connection accepted from client.\n");

    srand((unsigned int)time(NULL));
    char received[10000] = {0};  // To keep track of received frames

    while(1) {
        char buffer[128];
        memset(buffer, 0, sizeof(buffer));

        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            printf("Connection closed or error occurred. Ending session.\n");
            break;
        }

        int frame = atoi(buffer);
        printf("Received Frame %d...\n", frame);

        int f = rand() % 2;
        if (f == 0 && !received[frame]) { // Simulating frame loss and avoiding duplicate frames
            sprintf(buffer, "%d", frame);
            send(clientSocket, buffer, strlen(buffer), 0);
            printf("Acknowledgment sent for Frame %d...\n", frame);
            received[frame] = 1;
        } else {
            printf("Simulating frame drop for frame: %d\n", frame);
        }
    }
    closesocket(clientSocket);
    closesocket(listenSocket);
    WSACleanup();
    
}

