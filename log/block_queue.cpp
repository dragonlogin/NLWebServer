//
// Created by nl on 2020/9/9.
//
#include "block_queue.h"
template <class T>
BlockQueue<T>::BlockQueue(int max_size) {
    if (max_size <= 0)
    {
        exit(-1);
    }

    m_max_size = max_size;
    m_array = new T[max_size];
    m_size = 0;
    m_front = -1;
    m_back = -1;
}


template <class T>
void BlockQueue<T>::clear() {
    m_mutex.lock();
    m_size = 0;
    m_front = -1;
    m_back = -1;
    m_mutex.unlock();
}

template <class T>
bool BlockQueue<T>::full(){
    MutexLockGuard lock(m_mutex);
    return (m_size >= m_max_size);
}

template <class T>
bool BlockQueue<T>:: empty() {
    MutexLockGuard lock(m_mutex);
    return (0 == m_size);
}
template <class T>
bool BlockQueue<T>::front(T &value) {
    MutexLockGuard lock(m_mutex);
    return m_array[m_front];
}

template <class T>
bool BlockQueue<T>::back(T &value) {
    MutexLockGuard lock(m_mutex);
    return m_array[m_back];
}

template <class T>
int BlockQueue<T>::size() {
    MutexLockGuard lock(m_mutex);
    return m_size;
}

template <class T>
int BlockQueue<T>::max_size() {
    MutexLockGuard lock(m_mutex);
    return m_max_size;
}
template <class T>
bool BlockQueue<T>::push(const T &item) {
    MutexLockGuard lock(m_mutex);
    if (m_size >= m_max_size){

        m_cond.broadcast();
        return false;
    }
    m_back = (m_back + 1) % m_max_size;
    m_array[m_back] = item;
    m_size++;
    m_cond.broadcast();
    return true;

}
template <class T>
bool BlockQueue<T>::pop(T &item) {
    MutexLockGuard lock(m_mutex);
    while (m_size <= 0)
    {

        if (!m_cond.wait(m_mutex.get()))
        {
            return false;
        }
    }
    m_front = (m_front + 1) % m_max_size;
    item = m_array[m_front];
    m_size--;
    return true;
}
template <class T>
bool BlockQueue<T>::pop(T &item, int ms_timeout) {
    struct timespec t = {0, 0};
    struct timeval now = {0, 0};
    gettimeofday(&now, NULL);

    MutexLockGuard lock(m_mutex);
    if (m_size <= 0) {
        t.tv_sec = now.tv_sec + ms_timeout / 1000;
        t.tv_nsec = (ms_timeout % 1000) * 1000;
        if (!m_cond.timewait(m_mutex.get(), t)){
            return false;
        }
    }
    m_front = (m_front + 1) % m_max_size;
    item = m_array[m_front];
    m_size--;
    return true;
}
