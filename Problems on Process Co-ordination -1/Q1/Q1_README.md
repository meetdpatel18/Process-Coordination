# Q1: Articles and Editors problem

# How to run
command : 
g++ Q1.cpp
./a.out N M
N = number of editors
M = number of articles

## CONSTRAINTS:
1. Input must be positive

## Problem: Busy waiting
### Possible: YES
### Reason:
1. Consider there are n-1 articles and n editors.
2. Now lets say they are accessing the articles to read randomly,
but at any point of time only four of the editors will get 
articles to read.
3. Suppose, n-1 editors got the articles to read, one article to each 
editor and now there is one editor which is continously checking that is any article available for reading but it will not be available till any one of them completes the reading.
4. So, till these time it is continously checking for the articles availability.
5. These CPU cycles used for cheking the article availability continously is called busy waiting happened.
6. These will be true whenever there will be articles remaining for decision are less then no of editors to read where every article
which is remaining is the one which is not yet rejected by atleast
on of the editor.

## Problem: Deadlock
### Possible: NO
### Reason:
1. Here, at a time an editor can read only one article
2. So, no wait for article reading cycle (circular wait) is possible.

## Problem: Race condition
### Possible: YES
### Reason:
1. Here, at a time more than one editor can come for reading same article.
2. Now, article is a shared resource and because of that which article will be accepted by which editor
depends on the sequence in which article reading is done by the editors(race condition).
3. To synchronize these article reading semaphore locks are used.

## Problem: Starvation
### Possible: NO
### Reason:
1. Here, we are not setting the priority for any editor so whenever it will ask for the article to read
it will be assigned according to the arrival of the all the editors who have asked for same article reading.
2. So, it will never be the case that some editor 2 who comes after the editor 1 for taking lock on article and editor 2 gets the lock on that artilce before editor 1 gets lock on that same article.
