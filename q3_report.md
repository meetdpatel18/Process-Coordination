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


### SAMPLE INPUT
![Screenshot (143)](https://user-images.githubusercontent.com/110759150/204229346-dd7a89d1-1e8f-45e0-8c0f-4742fca17d5b.png)


### NORMAL IMPLEMENTATION
![Screenshot (149)](https://user-images.githubusercontent.com/110759150/204229658-0d2c6e7f-baa8-4153-8e22-a0244bef4393.png)

![Screenshot (144)](https://user-images.githubusercontent.com/110759150/204230061-6ee1cfb5-5e44-4647-b286-815fe3b23d35.png)


### SHARED MEMORY AND FORK IMPLEMENTATION
![Screenshot (145)](https://user-images.githubusercontent.com/110759150/204230471-27be7c9f-682f-4fcf-906e-eba5916e56c9.png)

![Screenshot (146)](https://user-images.githubusercontent.com/110759150/204230670-ca11c5a8-a5a9-4528-a654-f2486c233430.png)

### THREAD IMPLEMENTATION
![Screenshot (147)](https://user-images.githubusercontent.com/110759150/204231169-2f9e74b6-592b-4cd8-86e8-48cb912d9f63.png)

![Screenshot (148)](https://user-images.githubusercontent.com/110759150/204231502-8636103e-5631-49ed-8f72-7c7b42b93d26.png)
