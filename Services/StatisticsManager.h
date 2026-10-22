#ifndef STATISTICS_MANAGER_H
#define STATISTICS_MANAGER_H

#include "../DataStructures/HashTable.h"
#include "../Algorithms/MergeSort.h"
#include "../Editor/Document.h"
#include <string>
#include <vector>

/**
 * @brief Document statistics and frequency analyzer.
 */
class StatisticsManager {
public:
    struct DocumentStats {
        int wordCount;
        int charCountWithSpaces;
        int charCountWithoutSpaces;
        int lineCount;
        int paragraphCount;
        double averageWordLength;
        std::vector<std::pair<std::string, int>> topWords;

        DocumentStats()
            : wordCount(0), charCountWithSpaces(0), charCountWithoutSpaces(0),
              lineCount(0), paragraphCount(0), averageWordLength(0.0) {}
    };

    static DocumentStats computeStatistics(const Document& doc, size_t topNWords = 10);
    static HashTable<std::string, int> buildWordFrequencyTable(const std::string& text);
    static std::vector<std::pair<std::string, int>> getRankedFrequencies(const HashTable<std::string, int>& table);
};

#endif // STATISTICS_MANAGER_H
