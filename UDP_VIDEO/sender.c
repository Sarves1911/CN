#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <winsock2.h>
#include <openssl/aes.h>
#include <openssl/rand.h>

#define PORT 12345
#define MAX_BUFFER_SIZE 1024
#define WINDOW_SIZE 10

struct Packet
{
    uint32_t sequenceNumber;
    char data[MAX_BUFFER_SIZE - sizeof(uint32_t)];
};

// Function to encrypt data using AES-256 in CBC mode
void encrypt(const char *input, char *output, const AES_KEY *key, const unsigned char *iv)
{
    AES_cbc_encrypt(input, output, strlen(input), key, iv, AES_ENCRYPT);
}

int main()
{
    WSADATA wsa;
    SOCKET sockfd;
    struct sockaddr_in server_addr;
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

    // Open the video file for reading
    video_file = fopen("video.mp4", "rb");
    if (video_file == NULL)
    {
        printf("Error opening video file\n");
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    struct Packet packet;
    int windowStart = 0;
    int windowEnd = 0;
    int sequenceNumber = 0;

    // Set up AES encryption key and IV
    AES_KEY aesKey;
    unsigned char iv[AES_BLOCK_SIZE];
    RAND_bytes(iv, AES_BLOCK_SIZE);

    // Read video data from file and send encrypted packets with flow control
    while (windowEnd - windowStart < WINDOW_SIZE &&
           (bytesRead = fread(packet.data, 1, sizeof(packet.data), video_file)) > 0)
    {
        packet.sequenceNumber = htonl(sequenceNumber);

        // Encrypt the data before sending
        char encryptedData[MAX_BUFFER_SIZE - sizeof(uint32_t)];
        encrypt(packet.data, encryptedData, &aesKey, iv);

        // Copy the encrypted data into the packet
        memcpy(packet.data, encryptedData, sizeof(encryptedData));

        char serializedPacket[sizeof(packet)];
        memcpy(serializedPacket, &packet, sizeof(packet));
        sendto(sockfd, serializedPacket, sizeof(serializedPacket), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        windowEnd++;
        sequenceNumber++;
    }

    // Close the video file and socket
    fclose(video_file);
    closesocket(sockfd);
    WSACleanup();

    printf("Video sent successfully.\n");

    return 0;
}
