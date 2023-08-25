#include <semaphore.h>
#include <stdlib.h>
#include <random>
#include <pthread.h>
#include <stdbool.h>
#include <iostream>
#include <bits/stdc++.h>
#include "customlogger.h"

using namespace std;

// editorThreadIds[i] is for each editor
pthread_t *editorThreadIds;
// semaphore lock to avoid race condition on articles and articleLocks DS
sem_t *articleLocks_WLock,*articles_WLock,*articles_RLock;
int *articlesReadersCount;
// stores that is article[i] accepted or not
bool *articles;
// stores availability of mutex lock on each article i 
bool *articleLocks;

vector<vector<int>> res;
// N-> no of editors
// M-> no of articles
int N,M;

// it will check is lock available in given article NO
bool isLockAvailable(int articleNo){

    bool isAvailable = false;

    // take lock
    sem_wait(&articleLocks_WLock[articleNo]);

    isAvailable = articleLocks[articleNo];

    if(isAvailable){
        articleLocks[articleNo] = false;
    }

    // release lock
    sem_post(&articleLocks_WLock[articleNo]);

    return isAvailable;

}

// it will check that is article accepted by any editor or not
bool isArticleAccepted(int articleNo){

    bool isAccepted = false;

    // Lock the semaphore
    sem_wait(&articles_RLock[articleNo]);
    articlesReadersCount[articleNo]++;

    if(articlesReadersCount[articleNo] == 1){
        sem_wait(&articles_WLock[articleNo]);
    }

    // Unlock the semaphore
    sem_post(&articles_RLock[articleNo]);
    
    // reading here
    isAccepted = articles[articleNo];

    
    // Lock the semaphore
    sem_wait(&articles_RLock[articleNo]);
    articlesReadersCount[articleNo]--;

    if (articlesReadersCount[articleNo] == 0) {
        sem_post(&articles_WLock[articleNo]);
    }
 
    // UNLock the semaphore
    sem_post(&articles_RLock[articleNo]);

    return isAccepted;

}

// accept an article given by editor
void acceptAnArticle(int articleNo){

    // take a lock
    sem_wait(&articles_WLock[articleNo]);

    articles[articleNo] = true;

    // release lock
    sem_post(&articles_WLock[articleNo]);

}

// perform reading on given artile no
bool readAnArticle(int editor,int articleNo){

    LOGGER::INFO("Editor %d  is reading an article no : %d", editor, articleNo);

    // it takes one second to read an article
    usleep(1000000);

    // make algortihm to either accept/reject an article

    // Use current time as seed for random generator
    srand(time(0));

    int num = rand();

    // accept article
    if(num % 2 == 0){
        return true;
    }

    // reject article
    return false;

}

void releaseArticleLock(int articleNo){

    // take lock
    sem_wait(&articleLocks_WLock[articleNo]);

    // writing
    articleLocks[articleNo] = true;

    // release lock
    sem_post(&articleLocks_WLock[articleNo]);

}

// random generator function
int randomfunc(int j)
{
    return rand() % j;
}
 

void randomizeArticles(vector<int>& articleList){

    srand(unsigned(time(0)));

    // using built-in random generator
    random_shuffle(articleList.begin(), articleList.end());
  
    // using randomfunc
    random_shuffle(articleList.begin(), articleList.end(), randomfunc);

}

void* performEditorTask(void* param){

    int editor = * (int*) param;

    LOGGER::INFO("Reading started for editor %d",editor);

    vector<int> articleList(M);
    vector<int> rejectList;
    vector<int> acceptList;

    for(int i = 0;i < M; i++){
        articleList[i] = i+1;
    }

    randomizeArticles(articleList);

    bool isCompleted = false;

    while(!isCompleted){

        vector<int> removeList;
        vector<int> remainingList;

        for(int article : articleList){
            
            // first check for the lock on article
            if(isLockAvailable(article)){
                if(!isArticleAccepted(article)){
                    bool isAccepted = readAnArticle(editor,article);
                    if(isAccepted){
                        acceptAnArticle(article);
                        acceptList.push_back(article);
                        LOGGER::INFO("Editor %d accepted article no : %d", editor, article);
                    }else{
                        rejectList.push_back(article);
                        LOGGER::INFO("Editor %d rejected article no : %d", editor, article);
                    }
                }

                removeList.push_back(article);

                // relase lock on article
                releaseArticleLock(article);

            }
        }

        // do diff of articleList and removeList
        set_difference(articleList.begin(), articleList.end(), removeList.begin(),removeList.end(),
        inserter(remainingList,remainingList.begin()));

        articleList = remainingList;

        isCompleted = articleList.empty();

    }

    res[editor] = acceptList;

    LOGGER::INFO("Reading completed for editor %d", editor);

    pthread_exit(NULL);

}

void initGlobalDS(){

    // making seperate thread for each editor
    editorThreadIds = new pthread_t[N+1];
    articles = new bool[M+1];
    articleLocks = new bool[M+1];
    articlesReadersCount = new int[M+1];
    
    articles_RLock = new sem_t[M+1];
    articles_WLock = new sem_t[M+1];
    articleLocks_WLock = new sem_t[M+1];
    articleLocks = new bool[M+1];
    
    res.resize(N+1);

    for(int lock = 0; lock <= M; lock++){

        sem_init(&articleLocks_WLock[lock], 0, 1);
        sem_init(&articles_WLock[lock], 0, 1);
        sem_init(&articles_RLock[lock], 0, 1);

        // intially all articles are available for reading
        articleLocks[lock] = true;
        // intially no article is accepted/rejected by others
        articles[lock] = false;
        articlesReadersCount[lock] = 0;
    }
    
}

int main(int argc, char **argv){

    // N = no of editors
    // M = no of articles
    N = atoi(argv[1]);
    M = atoi(argv[2]);

    LOGGER::EnableFileOutput();

    initGlobalDS();

    LOGGER::INFO("Program started ");

    int *temp = new int[N+1];

    for(int editor = 1; editor <= N; editor++){

        temp[editor] = editor;
        pthread_create(&editorThreadIds[editor], NULL, performEditorTask, &temp[editor]);

    }

    for(int editor = 1; editor <= N; editor++){
        temp[editor] = editor;
        pthread_join(editorThreadIds[temp[editor]], NULL);
    }

    int acceptCount = 0;

    for(int i = 1;i <= N;i++){
        if(!res[i].empty()){
            string art = "";
            acceptCount += res[i].size();
            for(int x : res[i])
                art += to_string(x) + " ";
            LOGGER::INFO("Editor %d accepted %d articles : %s", i, res[i].size() , art.c_str());
            cout << endl;
        }
    }

    LOGGER::DEBUG("Total articles accepted => %d",acceptCount);

    LOGGER::INFO("Program ended ");

    return 0;
}
