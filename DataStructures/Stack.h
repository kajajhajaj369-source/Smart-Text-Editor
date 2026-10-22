#ifndef STACK_H
#define STACK_H

#include <stdexcept>
#include <cstddef>
#include <utility>

/**
 * @brief Custom Generic Stack implementation.
 * 
 * Concept: LIFO (Last-In, First-Out) Data Structure.
 * Applications:
 *  1. Editor Undo and Redo operations.
 *  2. Bracket syntax matching and validation.
 * 
 * Time Complexities:
 *  - push: O(1) amortized
 *  - pop: O(1)
 *  - top/peek: O(1)
 *  - isEmpty: O(1)
 *  - size: O(1)
 */
template <typename T>
class Stack {
private:
    T* elements;
    size_t count;
    size_t capacity;

    void resize(size_t newCapacity) {
        T* newArray = new T[newCapacity];
        for (size_t i = 0; i < count; ++i) {
            newArray[i] = std::move(elements[i]);
        }
        delete[] elements;
        elements = newArray;
        capacity = newCapacity;
    }

public:
    explicit Stack(size_t initialCapacity = 16)
        : count(0), capacity(initialCapacity) {
        if (capacity == 0) capacity = 16;
        elements = new T[capacity];
    }

    ~Stack() {
        delete[] elements;
    }

    // Copy constructor
    Stack(const Stack& other)
        : count(other.count), capacity(other.capacity) {
        elements = new T[capacity];
        for (size_t i = 0; i < count; ++i) {
            elements[i] = other.elements[i];
        }
    }

    // Copy assignment
    Stack& operator=(const Stack& other) {
        if (this != &other) {
            delete[] elements;
            count = other.count;
            capacity = other.capacity;
            elements = new T[capacity];
            for (size_t i = 0; i < count; ++i) {
                elements[i] = other.elements[i];
            }
        }
        return *this;
    }

    // Move constructor
    Stack(Stack&& other) noexcept
        : elements(other.elements), count(other.count), capacity(other.capacity) {
        other.elements = nullptr;
        other.count = 0;
        other.capacity = 0;
    }

    // Move assignment
    Stack& operator=(Stack&& other) noexcept {
        if (this != &other) {
            delete[] elements;
            elements = other.elements;
            count = other.count;
            capacity = other.capacity;
            other.elements = nullptr;
            other.count = 0;
            other.capacity = 0;
        }
        return *this;
    }

    void push(const T& item) {
        if (count >= capacity) {
            resize(capacity * 2);
        }
        elements[count++] = item;
    }

    void push(T&& item) {
        if (count >= capacity) {
            resize(capacity * 2);
        }
        elements[count++] = std::move(item);
    }

    T pop() {
        if (isEmpty()) {
            throw std::underflow_error("Stack is empty: cannot pop");
        }
        T item = std::move(elements[--count]);
        return item;
    }

    const T& top() const {
        if (isEmpty()) {
            throw std::underflow_error("Stack is empty: cannot peek top");
        }
        return elements[count - 1];
    }

    T& top() {
        if (isEmpty()) {
            throw std::underflow_error("Stack is empty: cannot peek top");
        }
        return elements[count - 1];
    }

    bool isEmpty() const {
        return count == 0;
    }

    size_t size() const {
        return count;
    }

    void clear() {
        count = 0;
    }
};

#endif // STACK_H
