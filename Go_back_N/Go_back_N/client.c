#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock.h>

#define SERVER_ADDR "127.0.0.1"
#define PORT 8080

void transmission(SOCKET sock, int N, int tf, int *tt) {
    int i = 1;
    char buffer[128];
    while (i <= tf) {
        int z = 0;
        for (int k = i; k < i + N && k <= tf; k++) {
            sprintf(buffer, "%d", k);
            send(sock, buffer, strlen(buffer), 0);
            printf("Sending Frame %d...\n", k);
            (*tt)++;
        }

        for (int k = i; k < i + N && k <= tf; k++) {
            fd_set readfds;
            struct timeval timeout;
            FD_ZERO(&readfds);
            FD_SET(sock, &readfds);
            timeout.tv_sec = 2;  // 2 seconds timeout for acknowledgment
            timeout.tv_usec = 0;

            int activity = select(sock + 1, &readfds, NULL, NULL, &timeout);

            // If timeout or error, break and retransmit
            if (activity <= 0) {
                printf("Timeout for acknowledgment of Frame %d...\n", k);
                break;
            }

            memset(buffer, 0, sizeof(buffer));
int bytesRead = recv(sock, buffer, sizeof(buffer) - 1, 0); // reserve 1 byte for the null terminator
if (bytesRead > 0) {
    buffer[bytesRead] = '\0'; // null-terminate the received data
}

            int ack = atoi(buffer);

            if (ack == k) {
                printf("Acknowledgment for Frame %d...\n", k);
                z++;
            } else {
                printf("Expected acknowledgment for Frame %d, but received for Frame %d\n", k, ack);
                break;
            }
        }

        i += z;
    }
}


int main() {
    int tf, N, tt = 0;

    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in serverAddr;

    if (WSAStartup(MAKEWORD(1,1), &wsaData) != 0) {
        printf("WSAStartup failed. Error: %d\n", WSAGetLastError());
        return 1;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("Socket creation failed. Error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    serverAddr.sin_port = htons(PORT);

    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Error connecting to server. Error: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    printf("Connected to the server.\n");

    printf("Enter the Total number of frames: ");
    scanf("%d", &tf);
    printf("Enter the Window Size: ");
    scanf("%d", &N);

    transmission(sock, N, tf, &tt);

    printf("Total number of frames which were sent and resent are: %d\n", tt);

    closesocket(sock);
    WSACleanup();
    return 0;
}
