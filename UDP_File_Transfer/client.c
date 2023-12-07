#include <stdio.h>
#include <winsock2.h>

#define SERVER_PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsaData;
    SOCKET udpSocket;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];
    int addr_size;

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    udpSocket = socket(AF_INET, SOCK_DGRAM, 0);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    addr_size = sizeof(serverAddr);

    while(1) {
        printf("[Client]: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strlen(buffer) - 1] = '\0';  // Remove newline
        sendto(udpSocket, buffer, strlen(buffer), 0, (SOCKADDR *)&serverAddr, addr_size);

        recvfrom(udpSocket, buffer, BUFFER_SIZE, 0, NULL, NULL);
        printf("[Server]: %s\n", buffer);
    }

    closesocket(udpSocket);
    WSACleanup();
    return 0;
}
