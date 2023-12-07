#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib library

#define PORT 9999
#define SIZE 1024

void send_file(FILE *fp, SOCKET sockfd)
{
    char buffer[SIZE];
    while (1)
    {
        size_t num_read = fread(buffer, 1, SIZE, fp);
        if (num_read == 0)
        {
            break;
        }

        int n = send(sockfd, buffer, num_read, 0);
        if (n < 0)
        {
            perror("[-]Error Writing to Socket\n");
            exit(EXIT_FAILURE);
        }
        else if (n == 0)
        {
            break;
        }
    }
}

int main()
{
    WSADATA wsa;
    SOCKET server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[SIZE] = {0};

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Failed. Error Code: %d", WSAGetLastError());
        return 1;
    }

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d", WSAGetLastError());
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("[-]Binding Error");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 10) < 0)
    {
        perror("[-]Error Listening");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1)
    {
        new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (new_socket == INVALID_SOCKET)
        {
            perror("[-]Accept Failed");
            exit(EXIT_FAILURE);
        }

        printf("Connected to Client: %s over Port: %d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

        FILE *fp = fopen("audioFile.mp3", "rb");
        if (fp == NULL)
        {
            perror("[-]Error in reading file\n");
            exit(EXIT_FAILURE);
        }

        send_file(fp, new_socket);
        printf("File Sent Successfully to Client: %s\n\n", inet_ntoa(address.sin_addr));
        closesocket(new_socket);
        fclose(fp);
    }

    closesocket(server_fd);
    WSACleanup();

    return 0;
}
