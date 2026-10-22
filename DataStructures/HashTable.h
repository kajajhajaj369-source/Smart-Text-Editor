#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <string>
#include <vector>
#include <utility>
#include <cstddef>
#include <stdexcept>

/**
 * @brief Custom Separate-Chaining Hash Table implementation.
 * 
 * Concept: Direct address mapping via hash function with linked lists for collision handling.
 * Applications:
 *  1. Word frequency calculation in Document Statistics.
 *  2. Fast keyword search indexing.
 *  3. Autocomplete dictionary lookups.
 * 
 * Collision Resolution: Separate Chaining (singly linked list in each bucket).
 * 
 * Time Complexities:
 *  - insert / search / remove: O(1) average, O(n) worst-case (all collisions).
 *  - load factor rehash: triggered when load_factor >= 0.75.
 */
template <typename K, typename V>
class HashTable {
public:
    struct HashNode {
        K key;
        V value;
        HashNode* next;

        HashNode(const K& k, const V& v) : key(k), value(v), next(nullptr) {}
    };

private:
    HashNode** buckets;
    size_t numBuckets;
    size_t numElements;
    const double maxLoadFactor = 0.75;

    size_t hashFunction(const std::string& key) const {
        size_t hashVal = 0;
        const size_t prime = 31;
        for (char ch : key) {
            hashVal = (hashVal * prime + static_cast<unsigned char>(ch)) % numBuckets;
        }
        return hashVal;
    }

    template <typename T>
    size_t hashFunction(const T& key) const {
        return (static_cast<size_t>(key) * 2654435761u) % numBuckets;
    }

    void rehash(size_t newBucketCount) {
        HashNode** newBuckets = new HashNode*[newBucketCount]();
        size_t oldNumBuckets = numBuckets;
        HashNode** oldBuckets = buckets;

        buckets = newBuckets;
        numBuckets = newBucketCount;
        numElements = 0;

        for (size_t i = 0; i < oldNumBuckets; ++i) {
            HashNode* curr = oldBuckets[i];
            while (curr != nullptr) {
                insert(curr->key, curr->value);
                HashNode* prev = curr;
                curr = curr->next;
                delete prev;
            }
        }
        delete[] oldBuckets;
    }

public:
    explicit HashTable(size_t initialBuckets = 31)
        : numBuckets(initialBuckets), numElements(0) {
        if (numBuckets == 0) numBuckets = 31;
        buckets = new HashNode*[numBuckets]();
    }

    ~HashTable() {
        clear();
        delete[] buckets;
    }

    HashTable(const HashTable& other)
        : numBuckets(other.numBuckets), numElements(0) {
        buckets = new HashNode*[numBuckets]();
        for (size_t i = 0; i < other.numBuckets; ++i) {
            HashNode* curr = other.buckets[i];
            while (curr != nullptr) {
                insert(curr->key, curr->value);
                curr = curr->next;
            }
        }
    }

    HashTable& operator=(const HashTable& other) {
        if (this != &other) {
            clear();
            delete[] buckets;

            numBuckets = other.numBuckets;
            numElements = 0;
            buckets = new HashNode*[numBuckets]();
            for (size_t i = 0; i < other.numBuckets; ++i) {
                HashNode* curr = other.buckets[i];
                while (curr != nullptr) {
                    insert(curr->key, curr->value);
                    curr = curr->next;
                }
            }
        }
        return *this;
    }

    void insert(const K& key, const V& value) {
        if (static_cast<double>(numElements + 1) / numBuckets > maxLoadFactor) {
            rehash(numBuckets * 2 + 1);
        }

        size_t index = hashFunction(key);
        HashNode* curr = buckets[index];

        while (curr != nullptr) {
            if (curr->key == key) {
                curr->value = value;
                return;
            }
            curr = curr->next;
        }

        HashNode* newNode = new HashNode(key, value);
        newNode->next = buckets[index];
        buckets[index] = newNode;
        numElements++;
    }

    bool search(const K& key, V& outValue) const {
        size_t index = hashFunction(key);
        HashNode* curr = buckets[index];

        while (curr != nullptr) {
            if (curr->key == key) {
                outValue = curr->value;
                return true;
            }
            curr = curr->next;
        }
        return false;
    }

    bool contains(const K& key) const {
        V temp;
        return search(key, temp);
    }

    void incrementFrequency(const K& key, int delta = 1) {
        if (static_cast<double>(numElements + 1) / numBuckets > maxLoadFactor) {
            rehash(numBuckets * 2 + 1);
        }

        size_t index = hashFunction(key);
        HashNode* curr = buckets[index];

        while (curr != nullptr) {
            if (curr->key == key) {
                curr->value += delta;
                return;
            }
            curr = curr->next;
        }

        HashNode* newNode = new HashNode(key, delta);
        newNode->next = buckets[index];
        buckets[index] = newNode;
        numElements++;
    }

    bool remove(const K& key) {
        size_t index = hashFunction(key);
        HashNode* curr = buckets[index];
        HashNode* prev = nullptr;

        while (curr != nullptr) {
            if (curr->key == key) {
                if (prev == nullptr) {
                    buckets[index] = curr->next;
                } else {
                    prev->next = curr->next;
                }
                delete curr;
                numElements--;
                return true;
            }
            prev = curr;
            curr = curr->next;
        }
        return false;
    }

    void clear() {
        for (size_t i = 0; i < numBuckets; ++i) {
            HashNode* curr = buckets[i];
            while (curr != nullptr) {
                HashNode* next = curr->next;
                delete curr;
                curr = next;
            }
            buckets[i] = nullptr;
        }
        numElements = 0;
    }

    size_t size() const {
        return numElements;
    }

    size_t bucketCount() const {
        return numBuckets;
    }

    bool isEmpty() const {
        return numElements == 0;
    }

    std::vector<std::pair<K, V>> getAllEntries() const {
        std::vector<std::pair<K, V>> entries;
        entries.reserve(numElements);
        for (size_t i = 0; i < numBuckets; ++i) {
            HashNode* curr = buckets[i];
            while (curr != nullptr) {
                entries.push_back({curr->key, curr->value});
                curr = curr->next;
            }
        }
        return entries;
    }
};

#endif // HASH_TABLE_H
