#include "Autocomplete.h"
#include <cctype>

Autocomplete::Autocomplete() : dictionary(127) {
    loadDefaultDictionary();
}

Autocomplete::~Autocomplete() {}

void Autocomplete::loadDefaultDictionary() {
    const std::vector<std::string> defaultWords = {
        "algorithm", "array", "binary", "bracket", "class", "compiler",
        "complete", "compile", "comparison", "computer", "data", "delete",
        "document", "double", "dynamic", "editor", "element", "enqueue",
        "dequeue", "function", "graph", "hash", "hashtable", "header",
        "include", "index", "insert", "integer", "iterator", "linear",
        "linkedlist", "memory", "merge", "mergesort", "node", "object",
        "pointer", "program", "queue", "quick", "quicksort", "recursion",
        "recursive", "redo", "replace", "return", "search", "separate",
        "sort", "stack", "statistics", "string", "structure", "template",
        "traversal", "tree", "undo", "variable", "vector", "virtual", "void"
    };

    for (const auto& w : defaultWords) {
        dictionary.insert(w, 1);
    }
}

void Autocomplete::addWord(const std::string& word, int frequency) {
    if (word.empty()) return;
    std::string lower;
    for (char c : word) {
        if (std::isalnum(static_cast<unsigned char>(c))) {
            lower += static_cast<char>(std::tolower(c));
        }
    }
    if (lower.length() >= 2) {
        dictionary.incrementFrequency(lower, frequency);
    }
}

void Autocomplete::updateFromDocument(const std::string& text) {
    std::string current;
    for (char c : text) {
        if (std::isalnum(static_cast<unsigned char>(c))) {
            current += static_cast<char>(std::tolower(c));
        } else {
            if (current.length() >= 2) {
                dictionary.incrementFrequency(current, 1);
            }
            current.clear();
        }
    }
    if (current.length() >= 2) {
        dictionary.incrementFrequency(current, 1);
    }
}

std::vector<std::string> Autocomplete::getSuggestions(const std::string& prefix, size_t maxResults) const {
    std::vector<std::string> results;
    if (prefix.empty()) return results;

    std::string lowerPrefix;
    for (char c : prefix) {
        lowerPrefix += static_cast<char>(std::tolower(c));
    }

    auto entries = dictionary.getAllEntries();
    std::vector<std::pair<std::string, int>> matches;

    for (const auto& entry : entries) {
        const std::string& word = entry.first;
        if (word.length() >= lowerPrefix.length()) {
            if (word.substr(0, lowerPrefix.length()) == lowerPrefix) {
                matches.push_back(entry);
            }
        }
    }

    MergeSort::sort(matches, [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
        if (a.second != b.second) {
            return a.second > b.second;
        }
        return a.first < b.first;
    });

    for (size_t i = 0; i < matches.size() && results.size() < maxResults; ++i) {
        results.push_back(matches[i].first);
    }

    return results;
}

void Autocomplete::clear() {
    dictionary.clear();
    loadDefaultDictionary();
}
