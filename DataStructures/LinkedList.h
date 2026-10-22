#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdexcept>
#include <cstddef>
#include <string>
#include <vector>

template <typename T>
struct Node {
    T data;
    Node* next;
    Node* prev;

    explicit Node(const T& val) : data(val), next(nullptr), prev(nullptr) {}
};

/**
 * @brief Custom Doubly-Linked List implementation.
 * 
 * Concept: Sequence of linear nodes linked by pointers.
 * Applications:
 *  1. Document representation where each node represents a line of text.
 *  2. Fast line insertion and deletion without shifting large contiguous memory.
 * 
 * Time Complexities:
 *  - push_back / push_front: O(1)
 *  - insert(index): O(n) (traversal to index)
 *  - removeAt(index): O(n) (traversal to index)
 *  - get(index): O(n)
 *  - size: O(1)
 */
template <typename T>
class LinkedList {
private:
    Node<T>* head;
    Node<T>* tail;
    size_t count;

    Node<T>* getNodeAt(size_t index) const {
        if (index >= count) {
            throw std::out_of_range("Index out of bounds in LinkedList");
        }
        if (index < count / 2) {
            Node<T>* curr = head;
            for (size_t i = 0; i < index; ++i) {
                curr = curr->next;
            }
            return curr;
        } else {
            Node<T>* curr = tail;
            for (size_t i = count - 1; i > index; --i) {
                curr = curr->prev;
            }
            return curr;
        }
    }

public:
    LinkedList() : head(nullptr), tail(nullptr), count(0) {}

    ~LinkedList() {
        clear();
    }

    LinkedList(const LinkedList& other) : head(nullptr), tail(nullptr), count(0) {
        Node<T>* curr = other.head;
        while (curr != nullptr) {
            push_back(curr->data);
            curr = curr->next;
        }
    }

    LinkedList& operator=(const LinkedList& other) {
        if (this != &other) {
            clear();
            Node<T>* curr = other.head;
            while (curr != nullptr) {
                push_back(curr->data);
                curr = curr->next;
            }
        }
        return *this;
    }

    void push_back(const T& val) {
        Node<T>* newNode = new Node<T>(val);
        if (tail == nullptr) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
        count++;
    }

    void push_front(const T& val) {
        Node<T>* newNode = new Node<T>(val);
        if (head == nullptr) {
            head = tail = newNode;
        } else {
            newNode->next = head;
            head->prev = newNode;
            head = newNode;
        }
        count++;
    }

    void insert(size_t index, const T& val) {
        if (index > count) {
            throw std::out_of_range("Insert index out of range");
        }
        if (index == 0) {
            push_front(val);
            return;
        }
        if (index == count) {
            push_back(val);
            return;
        }

        Node<T>* curr = getNodeAt(index);
        Node<T>* newNode = new Node<T>(val);
        newNode->prev = curr->prev;
        newNode->next = curr;
        curr->prev->next = newNode;
        curr->prev = newNode;
        count++;
    }

    void removeAt(size_t index) {
        if (index >= count) {
            throw std::out_of_range("Delete index out of range");
        }
        Node<T>* target = getNodeAt(index);
        if (target == head && target == tail) {
            head = tail = nullptr;
        } else if (target == head) {
            head = head->next;
            head->prev = nullptr;
        } else if (target == tail) {
            tail = tail->prev;
            tail->next = nullptr;
        } else {
            target->prev->next = target->next;
            target->next->prev = target->prev;
        }
        delete target;
        count--;
    }

    T& get(size_t index) {
        return getNodeAt(index)->data;
    }

    const T& get(size_t index) const {
        return getNodeAt(index)->data;
    }

    void set(size_t index, const T& val) {
        getNodeAt(index)->data = val;
    }

    void clear() {
        Node<T>* curr = head;
        while (curr != nullptr) {
            Node<T>* next = curr->next;
            delete curr;
            curr = next;
        }
        head = tail = nullptr;
        count = 0;
    }

    size_t size() const {
        return count;
    }

    bool isEmpty() const {
        return count == 0;
    }

    Node<T>* getHead() const {
        return head;
    }

    Node<T>* getTail() const {
        return tail;
    }

    std::vector<T> toVector() const {
        std::vector<T> result;
        result.reserve(count);
        Node<T>* curr = head;
        while (curr != nullptr) {
            result.push_back(curr->data);
            curr = curr->next;
        }
        return result;
    }
};

#endif // LINKED_LIST_H
