#include "StatisticsManager.h"
#include <cctype>
#include <algorithm>

HashTable<std::string, int> StatisticsManager::buildWordFrequencyTable(const std::string& text) {
    HashTable<std::string, int> table(61);
    std::string currentWord;

    for (size_t i = 0; i < text.length(); ++i) {
        char c = text[i];
        if (std::isalnum(static_cast<unsigned char>(c))) {
            currentWord += static_cast<char>(std::tolower(c));
        } else {
            if (!currentWord.empty()) {
                table.incrementFrequency(currentWord);
                currentWord.clear();
            }
        }
    }

    if (!currentWord.empty()) {
        table.incrementFrequency(currentWord);
    }

    return table;
}

std::vector<std::pair<std::string, int>> StatisticsManager::getRankedFrequencies(const HashTable<std::string, int>& table) {
    std::vector<std::pair<std::string, int>> entries = table.getAllEntries();

    MergeSort::sort(entries, [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
        if (a.second != b.second) {
            return a.second > b.second;
        }
        return a.first < b.first;
    });

    return entries;
}

StatisticsManager::DocumentStats StatisticsManager::computeStatistics(const Document& doc, size_t topNWords) {
    DocumentStats stats;

    stats.wordCount = doc.getRecursiveWordCount();
    stats.charCountWithSpaces = doc.getRecursiveCharCount(true);
    stats.charCountWithoutSpaces = doc.getRecursiveCharCount(false);
    stats.lineCount = static_cast<int>(doc.lineCount());

    std::vector<std::string> lines = doc.getAllLines();
    int paragraphs = 0;
    bool inParagraph = false;
    for (const auto& line : lines) {
        bool hasNonSpace = false;
        for (char c : line) {
            if (!std::isspace(static_cast<unsigned char>(c))) {
                hasNonSpace = true;
                break;
            }
        }
        if (hasNonSpace) {
            if (!inParagraph) {
                inParagraph = true;
                paragraphs++;
            }
        } else {
            inParagraph = false;
        }
    }
    stats.paragraphCount = (paragraphs == 0 && stats.wordCount > 0) ? 1 : paragraphs;

    if (stats.wordCount > 0) {
        stats.averageWordLength = static_cast<double>(stats.charCountWithoutSpaces) / stats.wordCount;
    } else {
        stats.averageWordLength = 0.0;
    }

    std::string fullText = doc.toPlainText();
    HashTable<std::string, int> freqTable = buildWordFrequencyTable(fullText);
    std::vector<std::pair<std::string, int>> ranked = getRankedFrequencies(freqTable);

    if (ranked.size() > topNWords) {
        ranked.resize(topNWords);
    }
    stats.topWords = ranked;

    return stats;
}
