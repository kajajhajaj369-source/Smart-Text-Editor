#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "../DataStructures/Queue.h"
#include "../Editor/Document.h"
#include <string>
#include <vector>

/**
 * @brief File handling service integrating custom Queue for Recent Files tracking.
 */
class FileManager {
private:
    Queue<std::string> recentFiles;
    const size_t maxRecentFiles = 10;

public:
    FileManager();
    ~FileManager();

    bool openFile(const std::string& filePath, Document& doc, std::string& errorMessage);
    bool saveFile(const std::string& filePath, const Document& doc, std::string& errorMessage);

    void addRecentFile(const std::string& filePath);
    std::vector<std::string> getRecentFiles() const;
    void clearRecentFiles();
};

#endif // FILE_MANAGER_H
