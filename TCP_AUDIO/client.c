#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib library

#define PORT 9999
#define SIZE 1024

void write_file(SOCKET sockfd)
{
    int n;
    FILE *fp;
    const char *filename = "recvAudio.mp3";
    char buffer[SIZE];

    fp = fopen(filename, "wb");
    if (fp == NULL)
    {
        perror("[-]Error Creating File");
        return;
    }

    while (1)
    {
        int n = recv(sockfd, buffer, SIZE, 0);
        if (n < 0)
        {
            perror("[-]Error Reading From Socket\n");
            break;
        }
        else if (n == 0)
        {
            break;
        }

        fwrite(buffer, 1, n, fp);
    }

    fclose(fp);
}

int main()
{
    WSADATA wsa;
    SOCKET sock = 0;
    struct sockaddr_in serv_addr;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Failed. Error Code: %d", WSAGetLastError());
        return 1;
    }

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d", WSAGetLastError());
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("[-]Connection Failed");
        return 1;
    }

    write_file(sock);
    printf("File Received From Server.\n");
    printf("Closing Connection...\n");

    // Cleanup and close the socket
    closesocket(sock);
    WSACleanup();

    return 0;
}
