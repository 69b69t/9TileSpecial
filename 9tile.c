#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
//#include <stdlib.h>
#define THREADS 16

uint64_t mcStepSeed(const uint64_t s, const uint64_t salt)
{
    return s * (s * 6364136223846793005ULL + 1442695040888963407ULL) + salt;
}

int mcFirstIsZero(const uint64_t s)
{
    //mod = 16;
    return (int)(((long)s >> 24) % 13) == 0;
}

uint64_t getChunkSeed(uint64_t ss, int x, int z)
{
    ss += x;
    ss = mcStepSeed(ss, z);
    ss = mcStepSeed(ss, x);
    ss = mcStepSeed(ss, z);
    return ss;
}

uint64_t getStartSalt(uint64_t ws)
{
    ws = mcStepSeed(ws, 7590731853067264053ULL);
    ws = mcStepSeed(ws, 7590731853067264053ULL);
    ws = mcStepSeed(ws, 7590731853067264053ULL);
    return ws;
}

uint64_t getStartSeed(uint64_t ws)
{
    return mcStepSeed(getStartSalt(ws), 0);
}

int mapSpecial(uint64_t ws)
{
    const uint64_t st = getStartSalt(ws);
    const uint64_t ss = getStartSeed(ws);

    int i, j;

    for (j = 0; j < 3; j++)
    {
        for (i = 0; i < 3; i++)
        {
            if (!mcFirstIsZero(getChunkSeed(ss, i-1, j-1)))
            {
                return 0;
            }
        }
    }
    return 1;
}

void * mapSpecialRange(uint64_t offset)
{
    uint64_t min = offset;
    uint64_t max = offset + (1LL << 32) - 1;
    for(uint64_t seed = min; seed <= max; seed++)
    {
        if(mapSpecial(seed))
        {
            printf("%lld\n", seed);
        }
    }
}

void spawnThreads(uint64_t offoffset)
{
    uint64_t offset = offoffset;

    pthread_t thread_id[THREADS];
    int err;
    int i;

    for(i = 0; i < THREADS; i++)
    {
        pthread_create(&thread_id[i], NULL, mapSpecialRange, offset);
        offset += (1ULL << 32);
    }

    for(i = 0; i < THREADS; i++)
    {
        pthread_join(thread_id[i], NULL);
    }
}


int main()
{
    printf("this configuration will do %lld seeds per block\n\n", THREADS * (1ULL << 32));
    int startblock, endblock;

    printf("block to start at: ");
    scanf("%d", &startblock);
    printf("block to end at: ");
    scanf("%d", &endblock);


    for(int i = startblock; i <= endblock; i++)
    {
        uint64_t chunkstart = THREADS * (uint64_t)i << 32;
        //printf("Started block #%d with offset %ld\n", i, chunkstart);
        spawnThreads(chunkstart);
    }
    return 0;
}
