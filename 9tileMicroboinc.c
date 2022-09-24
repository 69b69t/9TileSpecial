#include <stdio.h>
#include <stdint.h>

uint64_t mcStepSeed(const uint64_t s, const uint64_t salt)
{
    return s * (s * 6364136223846793005ULL + 1442695040888963407ULL) + salt;
}

int mcFirstIsZero(const uint64_t s)
{
    return (int)(((uint64_t)s >> 24) % 13) == 0;
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

int main(int argc, char **argv)
{
    int64_t startSeed = 0;
    int64_t endSeed = 0;

    //file stuff
    FILE* fp;
    fp = fopen(argv[2], "r");
    fscanf(fp, "%ld %ld", &startSeed, &endSeed);
    fclose(fp);

    for(int64_t seed = startSeed; seed <= endSeed; seed++)
    {
        if(mapSpecial(seed))
        {
            printf("%ld\n", seed);
        }
    }
    return 0;
}