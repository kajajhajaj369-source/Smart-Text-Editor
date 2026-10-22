#ifndef AUTOCOMPLETE_H
#define AUTOCOMPLETE_H

#include "../DataStructures/HashTable.h"
#include "../Algorithms/MergeSort.h"
#include <string>
#include <vector>

/**
 * @brief Autocomplete suggestion service.
 */
class Autocomplete {
private:
    HashTable<std::string, int> dictionary;

    void loadDefaultDictionary();

public:
    Autocomplete();
    ~Autocomplete();

    void addWord(const std::string& word, int frequency = 1);
    void updateFromDocument(const std::string& text);
    std::vector<std::string> getSuggestions(const std::string& prefix, size_t maxResults = 8) const;
    void clear();
};

#endif // AUTOCOMPLETE_H
