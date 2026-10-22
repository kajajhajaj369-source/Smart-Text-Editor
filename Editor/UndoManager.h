#ifndef UNDO_MANAGER_H
#define UNDO_MANAGER_H

#include "../DataStructures/Stack.h"
#include "EditorAction.h"

/**
 * @brief Stack-based Undo and Redo manager.
 */
class UndoManager {
private:
    Stack<EditorAction> undoStack;
    Stack<EditorAction> redoStack;
    bool isUndoingOrRedoing;

public:
    UndoManager();
    ~UndoManager();

    void recordAction(const EditorAction& action);
    bool canUndo() const;
    bool canRedo() const;

    EditorAction undo();
    EditorAction redo();

    void clear();

    size_t getUndoCount() const { return undoStack.size(); }
    size_t getRedoCount() const { return redoStack.size(); }

    bool getIsUndoingOrRedoing() const { return isUndoingOrRedoing; }
    void setIsUndoingOrRedoing(bool val) { isUndoingOrRedoing = val; }
};

#endif // UNDO_MANAGER_H
