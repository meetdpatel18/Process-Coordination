# Q2: Dish-Washing Problem

## CONSTRAINTS:
1. Number of taps must be greater than or equal to 2.

## To compile the program use command
### g++ --std=c++17 q2.cpp

## To run the program use command
### ./a.out m n

## Problem: Busy waiting
### Possible: YES
### Reason:
1.  There will be two cases where the busy waiting occurs,
    1)  When the student wants to acquire the tap
    2)  After acquiring the tap if the student wants to acquire the scrotch brite
2.  In the first case, the student first checks if the tapNumber which is randomly generated is already acquired or not, if it is already acquired it will go to the next tap and so on in the circular manner till it gets the free tap.
3.  In the second case, after getting the free tap, student first acquires lock on the tap so that no one case can use that particular tap until the student has washed all his/her utensils, after acquiring the lock on tap student checks whether its adjacent scrotch brite is available or not, if it is available it acquires lock on it till its completion and then releases it, but if the scrotch brite is already acquired by someone else it keeps busy waiting for its adjacent scrotch brite till it gets free scrotch brite.

## Problem: Deadlock
### Possible: NO
### Reason:
1.  Here, at a time only one scrotch brite is required by any student.
2.  Therefore, there will be no case of circular dependency and thus there will be no deadlock at all.

## Problem: Race condition
### Possible: YES
### Reason:
1.  In this problem, students can randomly acquire any taps/scrotch brite and hence output will vary every time the program is executed.
2.  Hence there will be race condition in this problem.

## Problem: Starvation
### Possible: NO
### Reason:
1.  Here, there is no varying priority for different students, i.e all the students have the same priority
2.  Hence there will be no case where a lower priority student is waiting to acquire tap/scrotch brite which is acquired by the higher priority student.
3.  So, there will be no starvation and student will definitely get the resources within a finite amount of time.