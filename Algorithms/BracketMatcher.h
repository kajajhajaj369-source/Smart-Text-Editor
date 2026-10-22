#ifndef BRACKET_MATCHER_H
#define BRACKET_MATCHER_H

#include "../DataStructures/Stack.h"
#include <string>
#include <vector>

/**
 * @brief Bracket matching and syntax validation algorithm using custom Stack.
 * 
 * Concept: LIFO matching of open and closing delimiters.
 * Applications:
 *  1. Matching pair identification for cursor highlight (e.g. '{' matching with '}').
 *  2. Syntactic balance validation across code/text.
 * 
 * Time Complexity: O(n) where n is text length.
 * Space Complexity: O(n) worst case for deeply nested brackets.
 */
class BracketMatcher {
public:
    struct BracketEntry {
        char character;
        int position;
        int line;
        int column;

        BracketEntry() : character('\0'), position(-1), line(-1), column(-1) {}
        BracketEntry(char c, int pos, int l, int col)
            : character(c), position(pos), line(l), column(col) {}
    };

    struct MatchResult {
        bool isBracket;
        int matchPosition;
        int matchLine;
        int matchColumn;

        MatchResult() : isBracket(false), matchPosition(-1), matchLine(-1), matchColumn(-1) {}
        MatchResult(bool bracket, int pos, int l, int col)
            : isBracket(bracket), matchPosition(pos), matchLine(l), matchColumn(col) {}
    };

    static bool isOpeningBracket(char c) {
        return c == '(' || c == '{' || c == '[';
    }

    static bool isClosingBracket(char c) {
        return c == ')' || c == '}' || c == ']';
    }

    static bool isMatchingPair(char open, char close) {
        return (open == '(' && close == ')') ||
               (open == '{' && close == '}') ||
               (open == '[' && close == ']');
    }

    static MatchResult findMatch(const std::string& text, int cursorPos) {
        if (cursorPos < 0 || cursorPos >= static_cast<int>(text.length())) {
            return MatchResult();
        }

        char target = text[cursorPos];

        if (!isOpeningBracket(target) && !isClosingBracket(target)) {
            if (cursorPos > 0 && (isOpeningBracket(text[cursorPos - 1]) || isClosingBracket(text[cursorPos - 1]))) {
                cursorPos--;
                target = text[cursorPos];
            } else {
                return MatchResult();
            }
        }

        if (isOpeningBracket(target)) {
            Stack<char> stack;
            stack.push(target);

            for (size_t i = cursorPos + 1; i < text.length(); ++i) {
                char c = text[i];
                if (isOpeningBracket(c)) {
                    stack.push(c);
                } else if (isClosingBracket(c)) {
                    if (stack.isEmpty()) return MatchResult();
                    char top = stack.pop();
                    if (isMatchingPair(top, c) && stack.isEmpty()) {
                        return MatchResult(true, static_cast<int>(i), 0, 0);
                    }
                }
            }
        } else if (isClosingBracket(target)) {
            Stack<char> stack;
            stack.push(target);

            for (int i = cursorPos - 1; i >= 0; --i) {
                char c = text[i];
                if (isClosingBracket(c)) {
                    stack.push(c);
                } else if (isOpeningBracket(c)) {
                    if (stack.isEmpty()) return MatchResult();
                    char top = stack.pop();
                    if (isMatchingPair(c, top) && stack.isEmpty()) {
                        return MatchResult(true, i, 0, 0);
                    }
                }
            }
        }

        return MatchResult(true, -1, 0, 0);
    }

    static bool validateBracketBalance(const std::string& text, std::string& errorMessage) {
        Stack<BracketEntry> stack;
        int line = 1;
        int col = 1;

        for (size_t i = 0; i < text.length(); ++i) {
            char c = text[i];
            if (c == '\n') {
                line++;
                col = 1;
                continue;
            }

            if (isOpeningBracket(c)) {
                stack.push(BracketEntry(c, static_cast<int>(i), line, col));
            } else if (isClosingBracket(c)) {
                if (stack.isEmpty()) {
                    errorMessage = "Unmatched closing bracket '" + std::string(1, c) + "' at line " +
                                   std::to_string(line) + ", column " + std::to_string(col);
                    return false;
                }
                BracketEntry top = stack.pop();
                if (!isMatchingPair(top.character, c)) {
                    errorMessage = "Mismatched bracket: opened with '" + std::string(1, top.character) +
                                   "' at line " + std::to_string(top.line) + " but closed with '" +
                                   std::string(1, c) + "' at line " + std::to_string(line);
                    return false;
                }
            }
            col++;
        }

        if (!stack.isEmpty()) {
            BracketEntry unclosed = stack.pop();
            errorMessage = "Unclosed bracket '" + std::string(1, unclosed.character) + "' from line " +
                           std::to_string(unclosed.line) + ", column " + std::to_string(unclosed.column);
            return false;
        }

        errorMessage = "All brackets are balanced and correctly matched.";
        return true;
    }
};

#endif // BRACKET_MATCHER_H
