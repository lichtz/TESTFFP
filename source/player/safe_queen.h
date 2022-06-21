//
// Created by zhaoy on 2021/9/24.
//

#ifndef LIVIDEO_SAFE_QUEEN_H
#define LIVIDEO_SAFE_QUEEN_H


#include <queue>
#include <pthread.h>

using namespace std;

template<typename T>
class SafeQueen{
public:
    SafeQueen(){
        pthread_mutex_init(&mutex,0);
        pthread_cond_init(&cond,0);

    }
    ~SafeQueen(){
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&cond);
    }
    /**
     *
     * @param value
     */
    void push(T value){
        pthread_mutex_lock(&mutex);
        if (work){
            q.push(value);
            pthread_cond_signal(&cond);
        } else{

        }
       pthread_mutex_unlock(&mutex);
    }

    int pop(T &value){
        int ret = 0;
        pthread_mutex_lock(&mutex);
        while (work && q.empty()){
            //等待
            pthread_cond_wait(&cond,&mutex);
        }
        if (!q.empty()){
            value = q.front();
            q.pop();
            ret =1;
        }
        pthread_mutex_unlock(&mutex);

        return ret;
    }

    void setWork(int work){
        pthread_mutex_lock(&mutex);
        this->work = work;
        pthread_mutex_unlock(&mutex);
    }

    int empty(){
        return  q.empty();
    }

    int size(){
        return  q.size();
    }

    void  clear(){

    }


//    void setReleaseCallback(void (*param)(T **)){
//
//
//    }

private:
    queue<T> q;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int work;
};



#endif //LIVIDEO_SAFE_QUEEN_H
