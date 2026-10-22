#include "FileManager.h"
#include <fstream>
#include <sstream>

FileManager::FileManager() : recentFiles(maxRecentFiles) {}

FileManager::~FileManager() {}

bool FileManager::openFile(const std::string& filePath, Document& doc, std::string& errorMessage) {
    std::ifstream inFile(filePath, std::ios::in | std::ios::binary);
    if (!inFile.is_open()) {
        errorMessage = "Could not open file: " + filePath;
        return false;
    }

    std::stringstream buffer;
    buffer << inFile.rdbuf();
    inFile.close();

    doc.loadFromPlainText(buffer.str());
    doc.setFilePath(filePath);
    doc.setModified(false);

    addRecentFile(filePath);
    return true;
}

bool FileManager::saveFile(const std::string& filePath, const Document& doc, std::string& errorMessage) {
    std::ofstream outFile(filePath, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!outFile.is_open()) {
        errorMessage = "Could not save file to: " + filePath;
        return false;
    }

    std::string content = doc.toPlainText();
    outFile.write(content.c_str(), content.length());
    outFile.close();

    return true;
}

void FileManager::addRecentFile(const std::string& filePath) {
    if (filePath.empty()) return;

    recentFiles.remove(filePath);

    if (recentFiles.size() >= maxRecentFiles) {
        recentFiles.dequeue();
    }

    recentFiles.enqueue(filePath);
}

std::vector<std::string> FileManager::getRecentFiles() const {
    std::vector<std::string> forward = recentFiles.toVector();
    std::vector<std::string> reversed;
    reversed.reserve(forward.size());
    for (int i = static_cast<int>(forward.size()) - 1; i >= 0; --i) {
        reversed.push_back(forward[i]);
    }
    return reversed;
}

void FileManager::clearRecentFiles() {
    recentFiles.clear();
}
