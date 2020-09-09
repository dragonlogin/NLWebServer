//
// Created by nl on 2020/9/9.
//
#ifndef NLWEBSERVER_LOCKER_H
#define NLWEBSERVER_LOCKER_H
#include "../common.h"

// 对信号量的 RAII封装
class Sem {
public:
    Sem();
    explicit Sem(int num);
    ~Sem();
    bool wait();
    bool post();
private:
    sem_t m_sem;
};

class Locker {
public:
    Locker();
    ~Locker();
    bool lock();
    bool unlock();
    pthread_mutex_t* get();
private:
    pthread_mutex_t m_mutex;
};


class Cond {
public:
    Cond();
    ~Cond();
    bool wait(pthread_mutex_t *m_mutex);
    bool timewait(pthread_mutex_t *m_mutex, struct timespec t);
    bool signal();
    bool broadcast();
private:
    pthread_cond_t m_cond;
};

class MutexLockGuard {
public:
    MutexLockGuard(Locker locker);
    ~MutexLockGuard();
private:
    Locker m_lock;
};
#endif //NLWEBSERVER_LOCKER_H
