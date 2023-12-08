

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#define WINDOW_SIZE 4
#define FRAME_COUNT 10
#define TIMEOUT_SEC 2

typedef struct
{
    int seq_num;
    bool ack_received;
} Frame;

void simulate_network_delay()
{
    // Simulate network delay
    sleep(1);
}

int main()
{
    srand(time(NULL));

    Frame frames[FRAME_COUNT];
    int next_frame_to_send = 0;
    int base = 0;

    // Initialize frames
    for (int i = 0; i < FRAME_COUNT; i++)
    {
        frames[i].seq_num = i;
        frames[i].ack_received = false;
    }

    while (base < FRAME_COUNT)
    {
        // Send new frames if window is not full
        while (next_frame_to_send < base + WINDOW_SIZE && next_frame_to_send < FRAME_COUNT)
        {
            printf("Sending frame %d\n", frames[next_frame_to_send].seq_num);
            simulate_network_delay();

            // Simulate ACK loss with a 20% probability
            if (rand() % 100 < 20)
            {
                printf("ACK for frame %d lost\n", frames[next_frame_to_send].seq_num);
            }
            else
            {
                frames[next_frame_to_send].ack_received = true;
            }

            next_frame_to_send++;
        }

        // Check for ACKs and slide the window
        for (int i = base; i < next_frame_to_send; i++)
        {
            if (frames[i].ack_received)
            {
                printf("ACK received for frame %d\n", frames[i].seq_num);
                base = i + 1;
            }
            else
            {
                printf("Timeout for frame %d, resending\n", frames[i].seq_num);
                simulate_network_delay();
                frames[i].ack_received = true; // Simulate successful resend
            }
        }
    }

    printf("All frames sent and acknowledged.\n");

    return 0;
}
