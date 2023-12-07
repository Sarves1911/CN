#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <winsock2.h>
#include <openssl/aes.h>

#define PORT 12345
#define MAX_BUFFER_SIZE 1024
#define WINDOW_SIZE 10

struct Packet
{
    uint32_t sequenceNumber;
    char data[MAX_BUFFER_SIZE - sizeof(uint32_t)];
};

// Function to decrypt data using AES-256 in CBC mode
void decrypt(const char *input, char *output, const AES_KEY *key, const unsigned char *iv)
{
    AES_cbc_encrypt(input, output, strlen(input), key, iv, AES_DECRYPT);
}

int main()
{
    WSADATA wsa;
    SOCKET sockfd;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_len = sizeof(client_addr);
    char serializedPacket[MAX_BUFFER_SIZE];
    FILE *video_file;
    int bytesRead;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Failed to initialize Winsock\n");
        return 1;
    }

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
    {
        printf("Failed to create socket\n");
        WSACleanup();
        return 1;
    }

    // Configure server address struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
    {
        printf("Bind failed with error code: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    // Open a new video file for writing
    video_file = fopen("received_video.mp4", "wb");
    if (video_file == NULL)
    {
        printf("Error creating video file\n");
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    struct Packet packet;
    int expectedSequenceNumber = 0;

    // Set up AES decryption key and IV
    AES_KEY aesKey;
    unsigned char iv[AES_BLOCK_SIZE];

    // Receive data and decode packets with flow control
    while (1)
    {
        bytesRead = recvfrom(sockfd, serializedPacket, sizeof(serializedPacket), 0, (struct sockaddr *)&client_addr, &client_addr_len);
        if (bytesRead <= 0)
        {
            // Handle receive errors
            break;
        }

        // Deserialize the received byte stream into the packet structure
        memcpy(&packet, serializedPacket, sizeof(packet));
        // Convert sequence number back to host byte order
        uint32_t sequenceNumber = ntohl(packet.sequenceNumber);

        // Check if the received packet is within the current window
        if (sequenceNumber == expectedSequenceNumber)
        {
            // Decrypt the data before writing to the video file
            char decryptedData[MAX_BUFFER_SIZE - sizeof(uint32_t)];
            decrypt(packet.data, decryptedData, &aesKey, iv);

            // Write decrypted data to the video file
            fwrite(decryptedData, 1, bytesRead - sizeof(uint32_t), video_file);

            // Update expected sequence number
            expectedSequenceNumber++;
        }

        // Handle other cases as needed...
    }

    // Close the video file and socket
    fclose(video_file);
    closesocket(sockfd);
    WSACleanup();

    printf("Video received and saved as received_video.mp4.\n");

    return 0;
}
