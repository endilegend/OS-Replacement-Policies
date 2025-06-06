#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>

// Structure used for the CLOCK page replacement algorithm
struct Clock
{
    bool star; // reference bit
    int num;   // page number
};

// Global variables to store hit and page fault rates for all algorithms
double otpHitRate;
double lruHitRate;
double fifoHitRate;
double clockHitRate;
double otpPFRate;
double lruPFRate;
double fifoPFRate;
double clockPFRate;

// Declare clockFrames as a global variable
struct Clock clockFrames[2000]; // Maximum size to match the history array

// Function prototypes for page replacement algorithms
int opt(int frameSize, int *all, int history[2000][frameSize]);
int lru(int frameSize, int *all, int history[2000][frameSize]);
int fifo(int frameSize, int *all, int history[2000][frameSize]);
int clockAlgo(int frameSize, int *all, int history[2000][frameSize], bool starHistory[2000][frameSize]);
int optimalHelper(int start, int frameSize, int *all, int *opt);

// Generates a random number between low and high
int randomInt(int low, int high)
{
    return low + rand() % (high - low + 1);
}

int main()
{
    int frameSize;
    printf("Enter frames allowed: ");
    scanf("%i", &frameSize); // Input frame size

    // Declare arrays for storing page references and history
    int all[2000];
    int optHistory[2000][frameSize];
    int fifoHistory[2000][frameSize];
    int lruHistory[2000][frameSize];
    int clockHistory[2000][frameSize];
    bool clockStarHistory[2000][frameSize];

    // Fill the page reference string with random values
    for (int i = 0; i < 2000; i++)
    {
        all[i] = randomInt(0, 512);
    }

    // Execute all page replacement algorithms
    opt(frameSize, all, optHistory);
    fifo(frameSize, all, fifoHistory);
    lru(frameSize, all, lruHistory);
    clockAlgo(frameSize, all, clockHistory, clockStarHistory);

    // Output the page fault rates
    printf("\nFrame Size: %i\n", frameSize);
    printf("Algorithm    PF Rate (%%)\n");
    printf("--------------------------\n");
    printf("OPT         %.2f%%\n", otpPFRate * 100);
    printf("FIFO        %.2f%%\n", fifoPFRate * 100);
    printf("LRU         %.2f%%\n", lruPFRate * 100);
    printf("CLOCK       %.2f%%\n", clockPFRate * 100);
    printf("--------------------------\n");

    // Snapshot view loop
    int snapshot;
    for (;;)
    {
        printf("Which frame snapshot to be displayed (Enter -1 to end program): ");
        scanf("%i", &snapshot);
        if (snapshot == -1)
        {
            return 1;
        }

        printf("Snapshot for frame %i:\n", snapshot);

        printf("OPT: ");
        for (int i = 0; i < frameSize; i++)
        {
            printf("%d ", optHistory[snapshot][i]);
        }
        printf("\n");

        printf("FIFO: ");
        for (int i = 0; i < frameSize; i++)
        {
            printf("%d ", fifoHistory[snapshot][i]);
        }
        printf("\n");

        printf("LRU: ");
        for (int i = 0; i < frameSize; i++)
        {
            printf("%d ", lruHistory[snapshot][i]);
        }
        printf("\n");

        printf("CLOCK: ");
        for (int i = 0; i < frameSize; i++)
        {
            if (clockHistory[snapshot][i] != -1)
            {
                printf("%d", clockHistory[snapshot][i]);
                if (clockStarHistory[snapshot][i])
                    printf("*");
                printf(" ");
            }
        }
        printf("\n");

        printf("\n");
    }

    return 0;
}

// OPTIMAL Page Replacement Algorithm
int opt(int frameSize, int *all, int history[2000][frameSize])
{
    int misses = 0;
    int cap = 0;
    int opt[frameSize];
    memset(opt, -1, sizeof(opt));
    bool hitMiss = false;

    for (int i = 0; i < 2000; i++)
    {
        for (int j = 0; j < frameSize; j++)
        {
            if (opt[j] == all[i]) // Check for hit
            {
                hitMiss = true;
                break;
            }
        }
        if (hitMiss == false)
        {
            misses += 1;
            if (cap < frameSize)
            {
                opt[cap] = all[i]; // Fill empty slot
                cap++;
            }
            else
            {
                int replace = optimalHelper(i, frameSize, all, opt); // Find optimal replacement
                opt[replace] = all[i];
            }
        }
        hitMiss = false;
        for (int k = 0; k < frameSize; k++)
        {
            history[i][k] = opt[k]; // Record current state
        }
    }
    otpPFRate = misses / 2000.00;
    return -1;
}

// Helper to find the index of the page to replace using Optimal Algorithm
int optimalHelper(int start, int frameSize, int *all, int *opt)
{
    struct Optimal
    {
        int num;
        int useIndex;
        int currIndex;
    };

    struct Optimal optimal[frameSize];

    for (int i = 0; i < frameSize; i++)
    {
        optimal[i].currIndex = i;
        optimal[i].num = opt[i];
        optimal[i].useIndex = 999999;
    }
    for (int j = 0; j < frameSize; j++)
    {
        for (int i = start; i < 2000; i++)
        {
            if (all[i] == optimal[j].num)
            {
                optimal[j].useIndex = i; // Find next use
                break;
            }
        }
    }

    int greatest = 0;
    int replace = 0;
    for (int i = 0; i < frameSize; i++)
    {
        if (optimal[i].useIndex > greatest)
        {
            greatest = optimal[i].useIndex;
            replace = i; // Replace page with farthest future use
        }
    }
    return replace;
}

// FIFO Page Replacement Algorithm
int fifo(int frameSize, int *all, int history[2000][frameSize])
{
    int fifo[frameSize];
    memset(fifo, -1, sizeof(fifo));
    bool hitMiss = false;
    int first = 0;
    int misses = 0;
    int cap = 0;

    for (int i = 0; i < 2000; i++)
    {
        for (int j = 0; j < frameSize; j++)
        {
            if (fifo[j] == all[i])
            {
                hitMiss = true;
                break;
            }
        }
        if (hitMiss == false)
        {
            misses++;
            if (cap < frameSize)
            {
                fifo[cap] = all[i]; // Fill empty frame
                cap++;
            }
            else
            {
                fifo[first] = all[i]; // Replace oldest page
                first = (first + 1) % frameSize;
            }
        }
        hitMiss = false;
        for (int k = 0; k < frameSize; k++)
        {
            history[i][k] = fifo[k];
        }
    }
    fifoPFRate = misses / 2000.00;
    return -1;
}

// LRU Page Replacement Algorithm
int lru(int frameSize, int *all, int history[2000][frameSize])
{
    int lru[frameSize];
    memset(lru, -1, sizeof(lru));
    bool hitMiss = false;
    int misses = 0;
    int cap = 0;
    int uses[frameSize]; // Track last used index

    for (int i = 0; i < 2000; i++)
    {
        for (int j = 0; j < frameSize; j++)
        {
            if (lru[j] == all[i])
            {
                hitMiss = true;
                uses[j] = i;
                break;
            }
        }
        if (hitMiss == false)
        {
            misses += 1;
            if (cap < frameSize)
            {
                lru[cap] = all[i];
                uses[cap] = i;
                cap++;
            }
            else
            {
                int least = 99999;
                int index = 0;
                for (int x = 0; x < frameSize; x++)
                {
                    if (uses[x] < least)
                    {
                        least = uses[x];
                        index = x; // Replace least recently used
                    }
                }
                lru[index] = all[i];
                uses[index] = i;
            }
        }
        hitMiss = false;
        for (int k = 0; k < frameSize; k++)
        {
            history[i][k] = lru[k];
        }
    }
    lruPFRate = misses / 2000.00;
    return -1;
}

// CLOCK Page Replacement Algorithm
int clockAlgo(int frameSize, int *all, int history[2000][frameSize], bool starHistory[2000][frameSize])
{
    // Use the global clockFrames array
    bool hitMiss = false;
    int misses = 0;
    int curr = 0; // Pointer for clock hand

    for (int i = 0; i < frameSize; i++)
    {
        clockFrames[i].num = -1;
        clockFrames[i].star = false; // Initialize reference bit
    }

    for (int i = 0; i < 2000; i++)
    {
        for (int j = 0; j < frameSize; j++)
        {
            if (clockFrames[j].num == all[i])
            {
                clockFrames[j].star = true; // Set reference bit
                hitMiss = true;
                break;
            }
        }

        if (hitMiss == false)
        {
            misses++;
            for (;;)
            {
                if (clockFrames[curr].star == false)
                {
                    clockFrames[curr].num = all[i]; // Replace this page
                    clockFrames[curr].star = true;
                    curr = (curr + 1) % frameSize;
                    break;
                }
                clockFrames[curr].star = false; // Clear and move forward
                curr = (curr + 1) % frameSize;
            }
        }

        for (int k = 0; k < frameSize; k++)
        {
            history[i][k] = clockFrames[k].num; // Record current state
            starHistory[i][k] = clockFrames[k].star;
        }
        hitMiss = false;
    }

    clockPFRate = misses / 2000.00;
    return -1;
}
