//
// Created by nl on 2020/9/9.
//

/*
 * 循环数组实现的阻塞队列，m_back = (m_back + 1) % m_max_size;
 * 线程安全，每个操作前都要先加互斥锁，操作完后，再解锁
 * */
#ifndef NLWEBSERVER_BLOCK_QUEUE_H
#define NLWEBSERVER_BLOCK_QUEUE_H

#include "../common.h"
#include "../lock/locker.h"

using namespace std;

template <class T>
class BlockQueue {
public:
    BlockQueue(int max_size = 1000);
    ~BlockQueue() = default;
    void clear();
    bool full();
    bool empty();
    bool front(T &value);
    bool back(T &value);
    int size();
    int max_size();
    bool push(const T &item);
    bool pop(T &item);
    bool pop(T &item, int ms_timeout);

private:
    mutable Locker m_mutex;
    Cond m_cond;

    shared_ptr<T> m_array;
    int m_size;
    int m_max_size;
    int m_front;
    int m_back;
};
#endif //NLWEBSERVER_BLOCK_QUEUE_H
