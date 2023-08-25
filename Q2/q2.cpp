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
vector<int> lastStudentIdTapAccessed;

//----------Scrotch Brite Variables-------------
vector<bool> freeScrotch;
// free Scrotch Semaphore
vector<sem_t> freeScrotchArrWriteLock;
vector<int> lastStudentIdScrotchAccessed;

//----------Student Variables-------------
int studentCount;
int liveStudentThreads = 0;
vector<pthread_t> studentThreadIds;
vector<int> studentStartTime;
vector<int> studentEndTime;

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
        lastStudentIdTapAccessed.push_back(-1);
    }

    // Initializing Scrotch variables
    for (int i = 0; i < tapCount - 1; i++)
    {
        sem_init(&freeScrotchArrWriteLock[i], 0, 1);
        freeScrotch.push_back(true);
        lastStudentIdScrotchAccessed.push_back(-1);
    }

    studentThreadIds.resize(studentCount);
    studentStartTime.resize(studentCount, 0);
    studentEndTime.resize(studentCount, 0);

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

void *assignStudentToTap(void *args)
{
    // student p acquires lock on any available tap
    // selects a student from the queue by acquiring lock on it and then pops the first student and assigns it to the free tap
    int studentId = *(int *)args;
    bool isWashed = false;
    vector<int> utensilWashOrder{0, 1, 2};

    // Generating a random order in which the student will wash the utensil
    std::shuffle(begin(utensilWashOrder), end(utensilWashOrder), std::default_random_engine());

    int j = 0;
    while (!isWashed)
    {
        srand(time(0));
        usleep(100);
        int tapNumber = (rand() % tapCount + j % tapCount) % tapCount;

        if (isTapAvailableThenTake(tapNumber))
        {
            // studentStartTime[i] will be null initially when a student comes to take the tap
            // whenever he acquires the tap assign the start time of the student with the last access time of this tap
            if (lastStudentIdTapAccessed[tapNumber] == -1)
            {
                studentStartTime[studentId] = 0;
            }
            else
            {
                // if this tap was previously acquired by some other student then the current student's start time becomes the endtime of the student who accessed this tap previously
                studentStartTime[studentId] = studentEndTime[lastStudentIdTapAccessed[tapNumber]];
            }

            // Updating the last StudentId who accessed the current acquired tap
            lastStudentIdTapAccessed[tapNumber] = studentId;

            int currTime = studentStartTime[studentId];

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
                            // if the scrotch which the student has acquired has not previously been used by any student then update the id to current studentId
                            if (lastStudentIdScrotchAccessed[0] == -1)
                            {
                                lastStudentIdScrotchAccessed[0] = studentId;
                            }
                            // if the scrotch was previously accessed than take the last accessed time of this scrotch and update the currTime for the student
                            else
                            {
                                currTime = max(currTime, studentEndTime[lastStudentIdScrotchAccessed[0]]);
                                lastStudentIdScrotchAccessed[0] = studentId;
                            }

                            // Mark Scrotch as taken so we wont busy wait for the next iteration to acquire the scrotch
                            scrotchTaken = true;

                            // we update the start time for the current utensil to the last time someone has used the scrotch
                            int startTime = currTime;
                            currTime = currTime + washWithScrotchTime[it];

                            string log = "Student " + to_string(studentId) + " Washed " + indexToUtensil[it] + " from tap " + to_string(tapNumber) + " using scrotch brite " + to_string(0) + " start at: " + to_string(startTime) + " finish at: " + to_string(currTime);
                            Logger::DEBUG(log.c_str());

                            studentEndTime[studentId] = currTime;

                            // We release the scrotch 0 as we have already used it so someone else adjacent can use it
                            releaseScrotch(0);

                            // We start washing the current utensil with water
                            startTime = currTime;
                            currTime = currTime + washWithTapTime[it];

                            log = "Student " + to_string(studentId) + " Washed " + indexToUtensil[it] + " from tap " + to_string(tapNumber) + " with water start at: " + to_string(startTime) + " and finish at: " + to_string(currTime);
                            Logger::DEBUG(log.c_str());
                            break;
                        }
                    }
                }
                isWashed = true;
                studentEndTime[studentId] = currTime;
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
                            // if the scrotch which the student has acquired has not previously been used by any student then update the id to current studentId
                            if (lastStudentIdScrotchAccessed[tapCount - 2] == -1)
                            {
                                lastStudentIdScrotchAccessed[tapCount - 2] = studentId;
                            }
                            // if the scrotch was previously accessed than take the last accessed time of this scrotch and update the currTime for the student
                            else
                            {
                                currTime = max(currTime, studentEndTime[lastStudentIdScrotchAccessed[tapCount - 2]]);
                                lastStudentIdScrotchAccessed[tapCount - 2] = studentId;
                            }

                            // Mark Scrotch as taken so we wont busy wait for the next iteration to acquire the scrotch
                            scrotchTaken = true;

                            // we update the start time for the current utensil to the last time someone has used the scrotch
                            int startTime = currTime;
                            currTime = currTime + washWithScrotchTime[it];

                            string log = "Student " + to_string(studentId) + " Washed " + indexToUtensil[it] + " from tap " + to_string(tapNumber) + " using scrotch brite " + to_string(tapCount - 2) + " start at: " + to_string(startTime) + " finish at: " + to_string(currTime);
                            Logger::DEBUG(log.c_str());

                            studentEndTime[studentId] = currTime;

                            // We release the scrotch (tapCount-2) as we have already used it so someone else adjacent can use it
                            releaseScrotch(tapCount - 2);

                            // We start washing the current utensil with water
                            startTime = currTime;
                            currTime = currTime + washWithTapTime[it];

                            log = "Student " + to_string(studentId) + " Washed " + indexToUtensil[it] + " from tap " + to_string(tapNumber) + " with water start at: " + to_string(startTime) + " and finish at: " + to_string(currTime);
                            Logger::DEBUG(log.c_str());
                            break;
                        }
                    }
                }
                isWashed = true;
                studentEndTime[studentId] = currTime;
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
                            // if the scrotch which the student has acquired has not previously been used by any student then update the id to current studentId
                            if (lastStudentIdScrotchAccessed[tapNumber - 1] == -1)
                            {
                                lastStudentIdScrotchAccessed[tapNumber - 1] = studentId;
                            }
                            // if the scrotch was previously accessed than take the last accessed time of this scrotch and update the currTime for the student
                            else
                            {
                                currTime = max(currTime, studentEndTime[lastStudentIdScrotchAccessed[tapNumber - 1]]);
                                lastStudentIdScrotchAccessed[tapNumber - 1] = studentId;
                            }

                            // Mark Scrotch as taken so we wont busy wait for the next iteration to acquire the scrotch
                            scrotchTaken = true;

                            // we update the start time for the current utensil to the last time someone has used the scrotch
                            int startTime = currTime;
                            currTime = currTime + washWithScrotchTime[it];

                            string log = "Student " + to_string(studentId) + " Washed " + indexToUtensil[it] + " from tap " + to_string(tapNumber) + " using scrotch brite " + to_string(tapNumber - 1) + " start at: " + to_string(startTime) + " finish at: " + to_string(currTime);
                            Logger::DEBUG(log.c_str());

                            studentEndTime[studentId] = currTime;

                            // We release the scrotch 0 as we have already used it so someone else adjacent can use it
                            releaseScrotch(tapNumber - 1);

                            // We start washing the current utensil with water
                            startTime = currTime;
                            currTime = currTime + washWithTapTime[it];

                            log = "Student " + to_string(studentId) + " Washed " + indexToUtensil[it] + " from tap " + to_string(tapNumber) + " with water start at: " + to_string(startTime) + " and finish at: " + to_string(currTime);
                            Logger::DEBUG(log.c_str());
                            break;
                        }
                        if (isScrotchAvailableThenTake(tapNumber))
                        {
                            // if the scrotch which the student has acquired has not previously been used by any student then update the id to current studentId
                            if (lastStudentIdScrotchAccessed[tapNumber] == -1)
                            {
                                lastStudentIdScrotchAccessed[tapNumber] = studentId;
                            }
                            // if the scrotch was previously accessed than take the last accessed time of this scrotch and update the currTime for the student
                            else
                            {
                                currTime = max(currTime, studentEndTime[lastStudentIdScrotchAccessed[tapNumber]]);
                                lastStudentIdScrotchAccessed[tapNumber] = studentId;
                            }

                            scrotchTaken = true;
                            isWashed = true;
                            int startTime = currTime;
                            currTime = currTime + washWithScrotchTime[it];
                            string log = "Student " + to_string(studentId) + " Washed " + indexToUtensil[it] + " from tap " + to_string(tapNumber) + " using scrotch brite " + to_string(tapNumber) + " start at: " + to_string(startTime) + " finish at: " + to_string(currTime);
                            Logger::DEBUG(log.c_str());
                            studentEndTime[studentId] = currTime;

                            releaseScrotch(tapNumber);

                            // after releasing the scrotch wash the current utensil
                            startTime = currTime;
                            currTime = currTime + washWithTapTime[it];
                            log = "Student " + to_string(studentId) + " Washed " + indexToUtensil[it] + " from tap " + to_string(tapNumber) + " with water start at: " + to_string(startTime) + " and finish at: " + to_string(currTime);
                            Logger::DEBUG(log.c_str());
                            break;
                        }
                    }
                }
                isWashed = true;
                studentEndTime[studentId] = currTime;
            }
            releaseTap(tapNumber);
        }
        j++;
    }
}

int main(int argc, char **argv)
{
    Logger::EnableFileOutput();
    tapCount = atoi(argv[1]);
    studentCount = atoi(argv[2]);

    if (tapCount <= 1)
    {
        Logger::DEBUG("There must be atleast two taps");
        return 0;
    }

    initSemaphores();

    vector<int> vect(studentCount);

    for (int i = 0; i < studentCount; i++)
    {
        vect[i] = i;
        pthread_create(&studentThreadIds[i], NULL, assignStudentToTap, &vect[i]);
    }
    for (int i = 0; i < studentCount; i++)
    {
        pthread_join(studentThreadIds[i], NULL);
    }

    return 0;
}