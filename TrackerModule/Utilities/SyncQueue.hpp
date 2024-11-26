// FP

#ifndef SYNCQUEUE_CPP
# define SYNCQUEUE_CPP

#include <queue>
#include <mutex>
#include <condition_variable>

/**
 * This class defines a simple thread-safe and synchronized queue.
*/
template <typename T>
class SyncQueue {
    private:
        std::queue<T> dataQueue;
        mutable std::mutex dataMutex;
        std::condition_variable availableData;

        /**
         * Waits until the queue has at least one element to be retrieved.
        */
        void waitForElementInQueue(void) {
            std::unique_lock<std::mutex> lock(dataMutex);
            while(dataQueue.empty())
                availableData.wait(lock);
        }
    public:
        /**
         * Gets first element without popping the queue.
        */
        T getFrontData(void) {
            waitForElementInQueue();
            return dataQueue.front();
        }

        /**
         * Gets last element added in the queue.
        */
        T getBackData(void) {
            waitForElementInQueue();
            return dataQueue.back();
        }
    
        void enqueue(T element) {
            std::lock_guard<std::mutex> lock(dataMutex);
            dataQueue.push(element);
            availableData.notify_one();
        }

        /**
         * Removes one element from the queue and returns a copy of it.
         * @return Least recent element added to the queue.
        */
        T dequeue(void) {
            waitForElementInQueue();
            T data = dataQueue.front();
            dataQueue.pop();
            return data;
        }

        /**
         * Removes one element from the queue. This function does not create a temporary variable with a copy of the data, so it can be used for large data to reduce temporary copies.
         * @param[out] element Reference to a variable where the element will be saved.
        */
        void dequeue(T &element) {
            waitForElementInQueue();
            element = dataQueue.front();
            dataQueue.pop();
        }

        bool isEmpty() {
            return dataQueue.empty();
        }

};
#endif