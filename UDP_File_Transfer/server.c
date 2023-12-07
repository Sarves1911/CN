#include <stdio.h>
#include <winsock2.h>

#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsaData;
    SOCKET udpSocket;
    struct sockaddr_in serverAddr, clientAddr;
    char buffer[BUFFER_SIZE];
    int addr_size;

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    udpSocket = socket(AF_INET, SOCK_DGRAM, 0);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(udpSocket, (SOCKADDR *)&serverAddr, sizeof(serverAddr));

    addr_size = sizeof(clientAddr);

    while(1) {
        recvfrom(udpSocket, buffer, BUFFER_SIZE, 0, (SOCKADDR *)&clientAddr, &addr_size);
        printf("[Peer]: %s\n", buffer);

        printf("[Server]: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strlen(buffer) - 1] = '\0';  // Remove newline
        sendto(udpSocket, buffer, strlen(buffer), 0, (SOCKADDR *)&clientAddr, addr_size);
    }

    closesocket(udpSocket);
    WSACleanup();
    return 0;
}
