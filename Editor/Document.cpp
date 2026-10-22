#include "Document.h"
#include <sstream>
#include <cctype>

Document::Document() : modified(false), filePath("") {
    lines.push_back("");
}

Document::~Document() {}

void Document::appendLine(const std::string& lineText) {
    lines.push_back(lineText);
    modified = true;
}

void Document::insertLine(size_t index, const std::string& lineText) {
    lines.insert(index, lineText);
    modified = true;
}

void Document::deleteLine(size_t index) {
    if (lines.size() <= 1) {
        lines.set(0, "");
    } else {
        lines.removeAt(index);
    }
    modified = true;
}

std::string Document::getLine(size_t index) const {
    if (index >= lines.size()) {
        return "";
    }
    return lines.get(index);
}

void Document::updateLine(size_t index, const std::string& lineText) {
    if (index < lines.size()) {
        lines.set(index, lineText);
        modified = true;
    }
}

size_t Document::lineCount() const {
    return lines.size();
}

bool Document::isEmpty() const {
    return lines.isEmpty() || (lines.size() == 1 && lines.get(0).empty());
}

void Document::clear() {
    lines.clear();
    lines.push_back("");
    modified = false;
    filePath = "";
}

void Document::loadFromPlainText(const std::string& fullText) {
    lines.clear();
    std::stringstream ss(fullText);
    std::string line;
    bool hasLine = false;

    while (std::getline(ss, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        lines.push_back(line);
        hasLine = true;
    }

    if (!hasLine) {
        lines.push_back("");
    }

    if (!fullText.empty() && (fullText.back() == '\n' || fullText.back() == '\r')) {
        lines.push_back("");
    }

    modified = false;
}

std::string Document::toPlainText() const {
    std::string result;
    Node<std::string>* curr = lines.getHead();
    while (curr != nullptr) {
        result += curr->data;
        if (curr->next != nullptr) {
            result += "\n";
        }
        curr = curr->next;
    }
    return result;
}

std::vector<std::string> Document::getAllLines() const {
    return lines.toVector();
}

int Document::countWordsInNodeRecursive(const Node<std::string>* node) {
    if (node == nullptr) {
        return 0;
    }

    int countInThisLine = 0;
    bool inWord = false;
    for (char c : node->data) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            if (!inWord) {
                inWord = true;
                countInThisLine++;
            }
        } else {
            inWord = false;
        }
    }

    return countInThisLine + countWordsInNodeRecursive(node->next);
}

int Document::countCharsInNodeRecursive(const Node<std::string>* node, bool includeSpaces) {
    if (node == nullptr) {
        return 0;
    }

    int charsInThisLine = 0;
    for (char c : node->data) {
        if (includeSpaces || !std::isspace(static_cast<unsigned char>(c))) {
            charsInThisLine++;
        }
    }

    if (node->next != nullptr && includeSpaces) {
        charsInThisLine += 1;
    }

    return charsInThisLine + countCharsInNodeRecursive(node->next, includeSpaces);
}

int Document::getRecursiveWordCount() const {
    return countWordsInNodeRecursive(lines.getHead());
}

int Document::getRecursiveCharCount(bool includeSpaces) const {
    return countCharsInNodeRecursive(lines.getHead(), includeSpaces);
}
