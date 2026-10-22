#ifndef DSA_LOGGER_H
#define DSA_LOGGER_H

#include <string>
#include <vector>
#include <functional>
#include <ctime>
#include <iomanip>
#include <sstream>

/**
 * @brief Structure representing a discrete Data Structure & Algorithm operation event.
 */
struct DsaEvent {
    std::string timestamp;
    std::string actionName;      // e.g. "Undo", "Open File", "Word Frequency"
    std::string dataStructure;   // e.g. "Stack", "Queue", "Linked List", "Hash Table", "Merge Sort"
    std::string operation;       // e.g. "POP", "PUSH", "ENQUEUE", "INSERT", "SEARCH", "MERGE"
    std::string complexity;      // e.g. "O(1)", "O(n)", "O(n log n)"
    std::string principle;       // e.g. "LIFO principle - reverses last operation first"

    DsaEvent() = default;
    DsaEvent(const std::string& act, const std::string& ds, const std::string& op,
             const std::string& comp, const std::string& prin)
        : actionName(act), dataStructure(ds), operation(op), complexity(comp), principle(prin) {
        auto now = std::time(nullptr);
        auto tm = *std::localtime(&now);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%H:%M:%S");
        timestamp = oss.str();
    }
};

/**
 * @brief Real-time DSA Audit Logger and Notification Manager.
 * 
 * Records every DSA operation performed by the editor and notifies
 * the Live Mapping HUD and Activity Log listeners.
 */
class DsaLogger {
private:
    std::vector<DsaEvent> eventHistory;
    std::function<void(const DsaEvent&)> onEventLogged;

    DsaLogger() = default;

public:
    static DsaLogger& instance() {
        static DsaLogger inst;
        return inst;
    }

    void log(const std::string& action, const std::string& ds, const std::string& op,
             const std::string& comp, const std::string& prin) {
        DsaEvent evt(action, ds, op, comp, prin);
        eventHistory.push_back(evt);
        if (onEventLogged) {
            onEventLogged(evt);
        }
    }

    void setListener(std::function<void(const DsaEvent&)> listener) {
        onEventLogged = listener;
    }

    const std::vector<DsaEvent>& getHistory() const {
        return eventHistory;
    }

    void clear() {
        eventHistory.clear();
    }
};

#endif // DSA_LOGGER_H
