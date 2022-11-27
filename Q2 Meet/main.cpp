#include <bits/stdc++.h>
#include <semaphore.h>
#include <pthread.h>
#include <algorithm>
#include <unistd.h>
#include "logger.h"

//----------Tap Variables-------------
int tapCount;
vector<bool> freeTaps;
// free Tap Semaphore
vector<sem_t> freeTapArrReadLock;
vector<sem_t> freeTapArrWriteLock;
vector<int> lastPersonIdTapAccessed;

//----------Scotch Brite Variables-------------
vector<bool> freeScotch;
// free Scotch Semaphore
vector<sem_t> freeScotchArrReadLock;
vector<sem_t> freeScotchArrWriteLock;
vector<int> lastPersonIdScotchAccessed;

//----------Person Variables-------------
int personCount;
int livePersonThreads = 0;
vector<pthread_t> personThreadIds;
vector<int> personStartTime;
vector<int> personEndTime;

vector<int> washWithScotchTime{4, 3, 2};
vector<int> washWithTapTime{5, 3, 1};

unordered_map<int, string> indexToUtensil;

class Wash
{
public:
    int startTime;
    int endTime;
    string res;

    Wash(int startTime, int endTime, string res)
    {
        this->startTime = startTime;
        this->endTime = endTime;
        this->res = res;
    }
};

bool comp(Wash &w1, Wash &w2)
{
    if (w1.startTime < w2.startTime)
        return true;
    else if (w1.startTime > w2.startTime)
        return false;

    return w1.endTime <= w2.endTime;
}

vector<Wash> resVect;
sem_t resVectReadLock;
sem_t resVectWriteLock;

void initSemaphores()
{
    // Initializing Tap variables
    for (int i = 0; i < tapCount; i++)
    {
        sem_init(&freeTapArrReadLock[i], 0, 1);
        sem_init(&freeTapArrWriteLock[i], 0, 1);
        freeTaps.push_back(true);
        lastPersonIdTapAccessed.push_back(-1);
    }

    // Initializing Scotch variables
    for (int i = 0; i < tapCount - 1; i++)
    {
        sem_init(&freeScotchArrReadLock[i], 0, 1);
        sem_init(&freeScotchArrWriteLock[i], 0, 1);
        freeScotch.push_back(true);
        lastPersonIdScotchAccessed.push_back(-1);
    }

    personThreadIds.resize(personCount);
    personStartTime.resize(personCount, 0);
    personEndTime.resize(personCount, 0);

    indexToUtensil[0] = "Plate";
    indexToUtensil[1] = "Glass";
    indexToUtensil[2] = "Spoon";

    sem_init(&resVectReadLock, 0, 1);
    sem_init(&resVectWriteLock, 0, 1);
}

bool isTapAvailableThenTake(int tapNumber)
{
    bool isAvailable = false;
    sem_wait(&freeTapArrWriteLock[tapNumber]);

    if (freeTaps[tapNumber])
    {
        freeTaps[tapNumber] = false;
        isAvailable = true;
    }

    sem_post(&freeTapArrReadLock[tapNumber]);
    return isAvailable;
}

void releaseTap(int tapNumber)
{
    sem_wait(&freeTapArrWriteLock[tapNumber]);

    freeTaps[tapNumber] = true;

    sem_post(&freeTapArrReadLock[tapNumber]);
}

bool isScotchAvailableThenTake(int scotchNumber)
{
    bool isAvailable = false;
    sem_wait(&freeScotchArrWriteLock[scotchNumber]);

    if (freeScotch[scotchNumber])
    {
        freeScotch[scotchNumber] = false;
        isAvailable = true;
    }

    sem_post(&freeScotchArrReadLock[scotchNumber]);
    return isAvailable;
}

void releaseScotch(int scotchNumber)
{
    sem_wait(&freeScotchArrWriteLock[scotchNumber]);

    freeScotch[scotchNumber] = true;

    sem_post(&freeScotchArrReadLock[scotchNumber]);
}

void pushToQueue(Wash w)
{
    sem_wait(&resVectWriteLock);
    resVect.push_back(w);
    sem_post(&resVectReadLock);
}

void performWash(int tapNumber, int scotchNumber, int personId, int &currTime, bool &scotchTaken, int utensilNumber)
{
    // if the scotch which the person has acquired has not previously been used by any person then update the id to current personId
    if (lastPersonIdScotchAccessed[scotchNumber] == -1)
    {
        lastPersonIdScotchAccessed[scotchNumber] = personId;
    }
    // if the scotch was previously accessed than take the last accessed time of this scotch and update the currTime for the person
    else
    {
        currTime = max(currTime, personEndTime[lastPersonIdScotchAccessed[scotchNumber]]);
        lastPersonIdScotchAccessed[scotchNumber] = personId;
    }

    scotchTaken = true;
    int startTime = currTime;
    currTime = currTime + washWithScotchTime[utensilNumber];

    string s = "Person " + to_string(personId) + " Washed " + indexToUtensil[utensilNumber] + " from tap " + to_string(tapNumber) + " using scotch brite " + to_string(scotchNumber) + " start at: " + to_string(startTime) + " finish at: " + to_string(currTime);
    Wash w(startTime, currTime, s);
    pushToQueue(w);
    // delete(w);

    personEndTime[personId] = currTime;
    releaseScotch(scotchNumber);

    startTime = currTime;
    // after releasing the scotch wash the current utensil
    currTime = currTime + washWithTapTime[utensilNumber];

    s = "Person " + to_string(personId) + " Washed " + indexToUtensil[utensilNumber] + " from tap " + to_string(tapNumber) + " with water start at: " + to_string(startTime) + " and finish at: " + to_string(currTime);
    Wash w1(startTime, currTime, s);
    pushToQueue(w1);
}

void *assignPersonToTap(void *args)
{
    // person p acquires lock on any available tap
    // selects a person from the queue by acquiring lock on it and then pops the first person and assigns it to the free tap
    int personId = *(int *)args;
    bool isWashed = false;
    vector<int> utensilWashOrder{0, 1, 2};
    std::shuffle(begin(utensilWashOrder), end(utensilWashOrder), std::default_random_engine());

    int j = 0;

    while (!isWashed)
    {
        srand(time(0));
        usleep(100);
        int tapNumber = (rand() % tapCount + j % tapCount) % tapCount;

        if (isTapAvailableThenTake(tapNumber))
        {
            // personStartTime[i] will be null initially when a person comes to take the tap
            // whenever he acquires the tap assign the start time of the person with the last access time of this tap
            if (lastPersonIdTapAccessed[tapNumber] == -1)
            {
                personStartTime[personId] = 0;
            }
            else
            {
                // if this tap was previously acquired by some other person then the current person's start time becomes the endtime of the person who accessed this tap previously
                personStartTime[personId] = personEndTime[lastPersonIdTapAccessed[tapNumber]];
            }

            lastPersonIdTapAccessed[tapNumber] = personId;

            int currTime = personStartTime[personId];

            if (tapNumber == 0)
            {
                // Wash utensil in a random order generated in the above logic on line 158
                for (auto it : utensilWashOrder)
                {
                    bool scotchTaken = false;
                    while (!scotchTaken && isScotchAvailableThenTake(0))
                    {
                        performWash(tapNumber, 0, personId, currTime, scotchTaken, it);
                        break;
                    }
                }
                isWashed = true;
                personEndTime[personId] = currTime;
            }
            else if (tapNumber == tapCount - 1)
            {
                for (auto it : utensilWashOrder)
                {
                    bool scotchTaken = false;
                    while (!scotchTaken && isScotchAvailableThenTake(tapCount - 2))
                    {
                        performWash(tapNumber, tapCount - 2, personId, currTime, scotchTaken, it);
                        break;
                    }
                }
                isWashed = true;
                personEndTime[personId] = currTime;
            }
            else
            {
                for (auto it : utensilWashOrder)
                {
                    bool scotchTaken = false;
                    while (!scotchTaken)
                    {
                        if (isScotchAvailableThenTake(tapNumber - 1))
                        {
                            performWash(tapNumber, tapNumber - 1, personId, currTime, scotchTaken, it);
                            break;
                        }
                        if (isScotchAvailableThenTake(tapNumber))
                        {
                            performWash(tapNumber, tapNumber, personId, currTime, scotchTaken, it);
                            break;
                        }
                    }
                }
                isWashed = true;
                personEndTime[personId] = currTime;
            }
            releaseTap(tapNumber);
        }
        j++;
    }
    pthread_exit(NULL);
}

int main()
{
    Logger::EnableFileOutput();
    // Logger::DEBUG("main called");

    cin >> tapCount >> personCount;

    initSemaphores();

    vector<int> vect(personCount);

    for (int i = 0; i < personCount; i++)
    {
        vect[i] = i;
        pthread_create(&personThreadIds[livePersonThreads++], NULL, assignPersonToTap, &vect[i]);
    }
    for (int i = 0; i < personCount; i++)
    {
        pthread_join(personThreadIds[i], NULL);
    }

    sort(resVect.begin(), resVect.end(), comp);

    for (auto it : resVect)
    {
        Logger::DEBUG((it.res).c_str());
    }

    return 0;
}