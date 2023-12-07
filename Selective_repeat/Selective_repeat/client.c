#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock.h>

#define SERVER_ADDR "127.0.0.1"
#define PORT 8080


// We'll use a struct to hold frames and their state
typedef struct {
    int seq_num;
    char data[128];
    int acked;
    int retries;
} Frame;

void transmission(SOCKET sock, int N, int tf, int *tt) {
    Frame window[N];  // Represents the current window of frames
    int base = 1, next_seq_num = 1;

    while (base <= tf) {
        while (next_seq_num < base + N && next_seq_num <= tf) {
            sprintf(window[next_seq_num % N].data, "%d", next_seq_num);
            window[next_seq_num % N].seq_num = next_seq_num;
            window[next_seq_num % N].acked = 0;
            window[next_seq_num % N].retries = 0;

            send(sock, window[next_seq_num % N].data, strlen(window[next_seq_num % N].data), 0);
            printf("Sending Frame %d...\n", next_seq_num);
            (*tt)++;
            next_seq_num++;
        }

        fd_set readfds;
        struct timeval timeout;
        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);
        timeout.tv_sec = 2;  // 2 seconds timeout for acknowledgment
        timeout.tv_usec = 0;

        select(sock + 1, &readfds, NULL, NULL, &timeout);

        if (FD_ISSET(sock, &readfds)) {
            char buffer[128];
            recv(sock, buffer, sizeof(buffer), 0);
            int ack = atoi(buffer);
            printf("Acknowledgment for Frame %d...\n", ack);
            window[ack % N].acked = 1;

            while (window[base % N].acked && base <= tf) {
                base++;
            }
        } else {
            // Timeout; Resend all unacknowledged frames
            for (int i = base; i < next_seq_num; i++) {
                if (!window[i % N].acked && window[i % N].retries < 3) { // Let's retry up to 3 times
                    send(sock, window[i % N].data, strlen(window[i % N].data), 0);
                    printf("Resending Frame %d...\n", i);
                    window[i % N].retries++;
                    (*tt)++;
                }
            }
        }
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
