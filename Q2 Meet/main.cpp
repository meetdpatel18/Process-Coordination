#include <bits/stdc++.h>
#include <semaphore.h>
#include <pthread.h>
#include <algorithm>
#include <unistd.h>
#include "logger.h"

using namespace std;

//----------Tap Variables-------------
int tapCount;
vector<bool> freeTaps;
// free Tap Semaphore
vector<sem_t> freeTapArrWriteLock;
vector<int> lastPersonIdTapAccessed;

//----------Scrotch Brite Variables-------------
vector<bool> freeScrotch;
// free Scrotch Semaphore
vector<sem_t> freeScrotchArrWriteLock;
vector<int> lastPersonIdScrotchAccessed;

//----------Person Variables-------------
int personCount;
int livePersonThreads = 0;
vector<pthread_t> personThreadIds;
vector<int> personStartTime;
vector<int> personEndTime;

vector<int> washWithScrotchTime{4, 3, 2};
vector<int> washWithTapTime{5, 3, 1};

unordered_map<int, string> indexToUtensil;

void initSemaphores()
{
    freeTapArrWriteLock.resize(tapCount);
    freeScrotchArrWriteLock.resize(tapCount - 1);

    // Initializing Tap variables
    for (int i = 0; i < tapCount; i++)
    {
        sem_init(&freeTapArrWriteLock[i], 0, 1);
        freeTaps.push_back(true);
        lastPersonIdTapAccessed.push_back(-1);
    }

    // Initializing Scrotch variables
    for (int i = 0; i < tapCount - 1; i++)
    {
        sem_init(&freeScrotchArrWriteLock[i], 0, 1);
        freeScrotch.push_back(true);
        lastPersonIdScrotchAccessed.push_back(-1);
    }

    personThreadIds.resize(personCount);
    personStartTime.resize(personCount, 0);
    personEndTime.resize(personCount, 0);

    indexToUtensil[0] = "Plate";
    indexToUtensil[1] = "Glass";
    indexToUtensil[2] = "Spoon";
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

    sem_post(&freeTapArrWriteLock[tapNumber]);
    return isAvailable;
}

void releaseTap(int tapNumber)
{
    sem_wait(&freeTapArrWriteLock[tapNumber]);

    freeTaps[tapNumber] = true;

    sem_post(&freeTapArrWriteLock[tapNumber]);
}

bool isScrotchAvailableThenTake(int scrotchNumber)
{
    bool isAvailable = false;
    sem_wait(&freeScrotchArrWriteLock[scrotchNumber]);

    if (freeScrotch[scrotchNumber])
    {
        freeScrotch[scrotchNumber] = false;
        isAvailable = true;
    }

    sem_post(&freeScrotchArrWriteLock[scrotchNumber]);
    return isAvailable;
}

void releaseScrotch(int scrotchNumber)
{
    sem_wait(&freeScrotchArrWriteLock[scrotchNumber]);

    freeScrotch[scrotchNumber] = true;

    sem_post(&freeScrotchArrWriteLock[scrotchNumber]);
}

void *assignPersonToTap(void *args)
{
    // person p acquires lock on any available tap
    // selects a person from the queue by acquiring lock on it and then pops the first person and assigns it to the free tap
    int personId = *(int *)args;
    bool isWashed = false;
    vector<int> utensilWashOrder{0, 1, 2};

    // Generating a random order in which the person will wash the utensil
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

            // Updating the last PersonId who accessed the current acquired tap
            lastPersonIdTapAccessed[tapNumber] = personId;

            int currTime = personStartTime[personId];

            if (tapNumber == 0)
            {
                // Start Washing Utensil in the random order generated above
                for (auto it : utensilWashOrder)
                {
                    bool scrotchTaken = false;
                    while (!scrotchTaken)
                    {
                        if (isScrotchAvailableThenTake(0))
                        {
                            // if the scrotch which the person has acquired has not previously been used by any person then update the id to current personId
                            if (lastPersonIdScrotchAccessed[0] == -1)
                            {
                                lastPersonIdScrotchAccessed[0] = personId;
                            }
                            // if the scrotch was previously accessed than take the last accessed time of this scrotch and update the currTime for the person
                            else
                            {
                                currTime = max(currTime, personEndTime[lastPersonIdScrotchAccessed[0]]);
                                lastPersonIdScrotchAccessed[0] = personId;
                            }

                            // Mark Scrotch as taken so we wont busy wait for the next iteration to acquire the scrotch
                            scrotchTaken = true;

                            // we update the start time for the current utensil to the last time someone has used the scrotch
                            int startTime = currTime;
                            currTime = currTime + washWithScrotchTime[it];

                            string log = "Person " + to_string(personId) + " Washed " + indexToUtensil[it] + " from tap " + to_string(tapNumber) + " using scrotch brite " + to_string(0) + " start at: " + to_string(startTime) + " finish at: " + to_string(currTime);
                            Logger::DEBUG(log.c_str());

                            personEndTime[personId] = currTime;

                            // We release the scrotch 0 as we have already used it so someone else adjacent can use it
                            releaseScrotch(0);

                            // We start washing the current utensil with water
                            startTime = currTime;
                            currTime = currTime + washWithTapTime[it];

                            log = "Person " + to_string(personId) + " Washed " + indexToUtensil[it] + " from tap " + to_string(tapNumber) + " with water start at: " + to_string(startTime) + " and finish at: " + to_string(currTime);
                            Logger::DEBUG(log.c_str());
                            break;
                        }
                    }
                }
                isWashed = true;
                personEndTime[personId] = currTime;
            }
            else if (tapNumber == tapCount - 1)
            {
                // Start Washing Utensil in the random order generated above

                for (auto it : utensilWashOrder)
                {
                    bool scrotchTaken = false;
                    while (!scrotchTaken)
                    {
                        if (isScrotchAvailableThenTake(tapCount - 2))
                        {
                            // if the scrotch which the person has acquired has not previously been used by any person then update the id to current personId
                            if (lastPersonIdScrotchAccessed[tapCount - 2] == -1)
                            {
                                lastPersonIdScrotchAccessed[tapCount - 2] = personId;
                            }
                            // if the scrotch was previously accessed than take the last accessed time of this scrotch and update the currTime for the person
                            else
                            {
                                currTime = max(currTime, personEndTime[lastPersonIdScrotchAccessed[tapCount - 2]]);
                                lastPersonIdScrotchAccessed[tapCount - 2] = personId;
                            }

                            // Mark Scrotch as taken so we wont busy wait for the next iteration to acquire the scrotch
                            scrotchTaken = true;

                            // we update the start time for the current utensil to the last time someone has used the scrotch
                            int startTime = currTime;
                            currTime = currTime + washWithScrotchTime[it];

                            string log = "Person " + to_string(personId) + " Washed " + indexToUtensil[it] + " from tap " + to_string(tapNumber) + " using scrotch brite " + to_string(tapCount - 2) + " start at: " + to_string(startTime) + " finish at: " + to_string(currTime);
                            Logger::DEBUG(log.c_str());

                            personEndTime[personId] = currTime;

                            // We release the scrotch (tapCount-2) as we have already used it so someone else adjacent can use it
                            releaseScrotch(tapCount - 2);

                            // We start washing the current utensil with water
                            startTime = currTime;
                            currTime = currTime + washWithTapTime[it];

                            log = "Person " + to_string(personId) + " Washed " + indexToUtensil[it] + " from tap " + to_string(tapNumber) + " with water start at: " + to_string(startTime) + " and finish at: " + to_string(currTime);
                            Logger::DEBUG(log.c_str());
                            break;
                        }
                    }
                }
                isWashed = true;
                personEndTime[personId] = currTime;
            }
            else
            {
                // Start Washing Utensil in the random order generated above
                for (auto it : utensilWashOrder)
                {
                    bool scrotchTaken = false;
                    while (!scrotchTaken)
                    {
                        if (isScrotchAvailableThenTake(tapNumber - 1))
                        {
                            // if the scrotch which the person has acquired has not previously been used by any person then update the id to current personId
                            if (lastPersonIdScrotchAccessed[tapNumber - 1] == -1)
                            {
                                lastPersonIdScrotchAccessed[tapNumber - 1] = personId;
                            }
                            // if the scrotch was previously accessed than take the last accessed time of this scrotch and update the currTime for the person
                            else
                            {
                                currTime = max(currTime, personEndTime[lastPersonIdScrotchAccessed[tapNumber - 1]]);
                                lastPersonIdScrotchAccessed[tapNumber - 1] = personId;
                            }

                            // Mark Scrotch as taken so we wont busy wait for the next iteration to acquire the scrotch
                            scrotchTaken = true;

                            // we update the start time for the current utensil to the last time someone has used the scrotch
                            int startTime = currTime;
                            currTime = currTime + washWithScrotchTime[it];

                            string log = "Person " + to_string(personId) + " Washed " + indexToUtensil[it] + " from tap " + to_string(tapNumber) + " using scrotch brite " + to_string(tapNumber - 1) + " start at: " + to_string(startTime) + " finish at: " + to_string(currTime);
                            Logger::DEBUG(log.c_str());

                            personEndTime[personId] = currTime;

                            // We release the scrotch 0 as we have already used it so someone else adjacent can use it
                            releaseScrotch(tapNumber - 1);

                            // We start washing the current utensil with water
                            startTime = currTime;
                            currTime = currTime + washWithTapTime[it];

                            log = "Person " + to_string(personId) + " Washed " + indexToUtensil[it] + " from tap " + to_string(tapNumber) + " with water start at: " + to_string(startTime) + " and finish at: " + to_string(currTime);
                            Logger::DEBUG(log.c_str());
                            break;
                        }
                        if (isScrotchAvailableThenTake(tapNumber))
                        {
                            // if the scrotch which the person has acquired has not previously been used by any person then update the id to current personId
                            if (lastPersonIdScrotchAccessed[tapNumber] == -1)
                            {
                                lastPersonIdScrotchAccessed[tapNumber] = personId;
                            }
                            // if the scrotch was previously accessed than take the last accessed time of this scrotch and update the currTime for the person
                            else
                            {
                                currTime = max(currTime, personEndTime[lastPersonIdScrotchAccessed[tapNumber]]);
                                lastPersonIdScrotchAccessed[tapNumber] = personId;
                            }

                            scrotchTaken = true;
                            isWashed = true;
                            int startTime = currTime;
                            currTime = currTime + washWithScrotchTime[it];
                            string log = "Person " + to_string(personId) + " Washed " + indexToUtensil[it] + " from tap " + to_string(tapNumber) + " using scrotch brite " + to_string(tapNumber) + " start at: " + to_string(startTime) + " finish at: " + to_string(currTime);
                            Logger::DEBUG(log.c_str());
                            personEndTime[personId] = currTime;

                            releaseScrotch(tapNumber);

                            // after releasing the scrotch wash the current utensil
                            startTime = currTime;
                            currTime = currTime + washWithTapTime[it];
                            log = "Person " + to_string(personId) + " Washed " + indexToUtensil[it] + " from tap " + to_string(tapNumber) + " with water start at: " + to_string(startTime) + " and finish at: " + to_string(currTime);
                            Logger::DEBUG(log.c_str());
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
}

int main()
{
    Logger::EnableFileOutput();

    cin >> tapCount >> personCount;

    initSemaphores();

    vector<int> vect(personCount);

    for (int i = 0; i < personCount; i++)
    {
        vect[i] = i;
        pthread_create(&personThreadIds[i], NULL, assignPersonToTap, &vect[i]);
    }
    for (int i = 0; i < personCount; i++)
    {
        pthread_join(personThreadIds[i], NULL);
    }

    return 0;
}