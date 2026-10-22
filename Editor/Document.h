#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "../DataStructures/LinkedList.h"
#include <string>
#include <vector>

/**
 * @brief Custom Document model representing text as a Linked List of lines.
 */
class Document {
private:
    LinkedList<std::string> lines;
    bool modified;
    std::string filePath;

    static int countWordsInNodeRecursive(const Node<std::string>* node);
    static int countCharsInNodeRecursive(const Node<std::string>* node, bool includeSpaces);

public:
    Document();
    ~Document();

    void appendLine(const std::string& lineText);
    void insertLine(size_t index, const std::string& lineText);
    void deleteLine(size_t index);
    std::string getLine(size_t index) const;
    void updateLine(size_t index, const std::string& lineText);

    size_t lineCount() const;
    bool isEmpty() const;
    void clear();

    void loadFromPlainText(const std::string& fullText);
    std::string toPlainText() const;
    std::vector<std::string> getAllLines() const;

    bool isModified() const { return modified; }
    void setModified(bool mod) { modified = mod; }

    const std::string& getFilePath() const { return filePath; }
    void setFilePath(const std::string& path) { filePath = path; }

    const Node<std::string>* getHeadNode() const { return lines.getHead(); }

    int getRecursiveWordCount() const;
    int getRecursiveCharCount(bool includeSpaces) const;
};

#endif // DOCUMENT_H
