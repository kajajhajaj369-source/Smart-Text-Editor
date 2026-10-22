#ifndef SEARCH_H
#define SEARCH_H

#include <string>
#include <vector>
#include <cctype>

/**
 * @brief Search and text match algorithms.
 */
class Search {
public:
    struct SearchMatch {
        int lineIndex;
        int startColumn;
        int length;
        int globalIndex;

        SearchMatch(int line, int col, int len, int gIdx = -1)
            : lineIndex(line), startColumn(col), length(len), globalIndex(gIdx) {}
    };

    static bool recursiveMatch(const std::string& text, size_t textPos,
                               const std::string& pattern, size_t patternPos,
                               bool matchCase) {
        if (patternPos == pattern.length()) {
            return true;
        }
        if (textPos == text.length()) {
            return false;
        }

        char tChar = matchCase ? text[textPos] : static_cast<char>(std::tolower(text[textPos]));
        char pChar = matchCase ? pattern[patternPos] : static_cast<char>(std::tolower(pattern[patternPos]));

        if (tChar == pChar) {
            return recursiveMatch(text, textPos + 1, pattern, patternPos + 1, matchCase);
        }
        return false;
    }

    static std::vector<int> findAllInString(const std::string& text, const std::string& pattern, bool matchCase) {
        std::vector<int> positions;
        if (pattern.empty() || text.length() < pattern.length()) {
            return positions;
        }

        size_t maxSearchPos = text.length() - pattern.length();
        for (size_t i = 0; i <= maxSearchPos; ++i) {
            if (recursiveMatch(text, i, pattern, 0, matchCase)) {
                positions.push_back(static_cast<int>(i));
            }
        }
        return positions;
    }

    static std::vector<SearchMatch> searchInLines(const std::vector<std::string>& lines,
                                                  const std::string& pattern,
                                                  bool matchCase) {
        std::vector<SearchMatch> matches;
        if (pattern.empty()) return matches;

        int globalOffset = 0;
        for (size_t lineIdx = 0; lineIdx < lines.size(); ++lineIdx) {
            const std::string& line = lines[lineIdx];
            std::vector<int> cols = findAllInString(line, pattern, matchCase);
            for (int col : cols) {
                matches.emplace_back(static_cast<int>(lineIdx), col, static_cast<int>(pattern.length()), globalOffset + col);
            }
            globalOffset += static_cast<int>(line.length()) + 1;
        }
        return matches;
    }
};

#endif // SEARCH_H
