#ifndef QUEUE_H
#define QUEUE_H

#include <stdexcept>
#include <cstddef>
#include <vector>

/**
 * @brief Custom Generic Queue implementation using circular array buffer.
 * 
 * Concept: FIFO (First-In, First-Out) Data Structure.
 * Applications:
 *  1. Recent Files history management (limiting to N items, removing oldest when full).
 * 
 * Time Complexities:
 *  - enqueue: O(1)
 *  - dequeue: O(1)
 *  - front: O(1)
 *  - isEmpty: O(1)
 *  - size: O(1)
 */
template <typename T>
class Queue {
private:
    T* elements;
    size_t head;
    size_t tail;
    size_t count;
    size_t capacity;

    void resize(size_t newCapacity) {
        T* newArray = new T[newCapacity];
        for (size_t i = 0; i < count; ++i) {
            newArray[i] = elements[(head + i) % capacity];
        }
        delete[] elements;
        elements = newArray;
        head = 0;
        tail = count;
        capacity = newCapacity;
    }

public:
    explicit Queue(size_t maxCapacity = 10)
        : head(0), tail(0), count(0), capacity(maxCapacity) {
        if (capacity == 0) capacity = 10;
        elements = new T[capacity];
    }

    ~Queue() {
        delete[] elements;
    }

    // Copy constructor
    Queue(const Queue& other)
        : head(0), tail(other.count), count(other.count), capacity(other.capacity) {
        elements = new T[capacity];
        for (size_t i = 0; i < count; ++i) {
            elements[i] = other.elements[(other.head + i) % other.capacity];
        }
    }

    // Copy assignment
    Queue& operator=(const Queue& other) {
        if (this != &other) {
            delete[] elements;
            count = other.count;
            capacity = other.capacity;
            head = 0;
            tail = count;
            elements = new T[capacity];
            for (size_t i = 0; i < count; ++i) {
                elements[i] = other.elements[(other.head + i) % other.capacity];
            }
        }
        return *this;
    }

    void enqueue(const T& item) {
        if (count >= capacity) {
            resize(capacity * 2);
        }
        elements[tail] = item;
        tail = (tail + 1) % capacity;
        count++;
    }

    T dequeue() {
        if (isEmpty()) {
            throw std::underflow_error("Queue is empty: cannot dequeue");
        }
        T item = elements[head];
        head = (head + 1) % capacity;
        count--;
        return item;
    }

    const T& front() const {
        if (isEmpty()) {
            throw std::underflow_error("Queue is empty: cannot peek front");
        }
        return elements[head];
    }

    bool isEmpty() const {
        return count == 0;
    }

    size_t size() const {
        return count;
    }

    void clear() {
        head = 0;
        tail = 0;
        count = 0;
    }

    std::vector<T> toVector() const {
        std::vector<T> result;
        result.reserve(count);
        for (size_t i = 0; i < count; ++i) {
            result.push_back(elements[(head + i) % capacity]);
        }
        return result;
    }

    bool contains(const T& item) const {
        for (size_t i = 0; i < count; ++i) {
            if (elements[(head + i) % capacity] == item) {
                return true;
            }
        }
        return false;
    }

    bool remove(const T& item) {
        if (isEmpty()) return false;
        std::vector<T> temp;
        bool found = false;
        while (!isEmpty()) {
            T current = dequeue();
            if (!found && current == item) {
                found = true;
            } else {
                temp.push_back(current);
            }
        }
        for (const auto& elem : temp) {
            enqueue(elem);
        }
        return found;
    }
};

#endif // QUEUE_H
