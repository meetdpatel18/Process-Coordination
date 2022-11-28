## CONSTRAINTS
Input can be positive or negative number

# SHARED MEMORY AND FORK IMPLEMENTATION
- Input size must not be too large (~1000) because large input size will lead to more fork calls and will  eventually lead to segmentation fault as main memory is limited
- We created a shared memory using SHMGET,SHMAT of size equal to input size of an array
- Then we are recursively calling fork for left and right child for MERGESORT IMPLEMENTATION
- We will wait for child process to complete then we will merge
- Then shared memory is detached after sorting using SHMDT,SHMCTL 


# THREAD IMPLEMENTATION
- Input size must not be too large (~1000) because large input will lead to creation of too many threads which will eventually lead to segmentation fault because threads per process are limited in linux
- New thread will be created for each mergesort recursive call 
- We will wait for recursive calls to complete before merging them
- I have declared a vector globally so that all threads can share this


# PERFORMANCE REPORT
- Thread implementation is faster than fork implementation:
    - Because IN FORK IMPLEMENTATION for both right and left child processes there will be cache miss 
     this will lead to degradation in performance 
    - In THREAD IMPLEMENTATION recursive calls can access vector at the same time concurrently because it is declared in a global scope and there will not be cache miss      for recursive calls

## FINAL RESULT
- NORMAL IMPLEMENTAION > THREAD IMPLEMENTATION > FORK IMPLEMENTATION

- NORMAL IMPLEMENTATION is FASTER than THREAD AND FORK 
- THREAD IMPLEMENTATION is FASTER than FORK
