#include "UndoManager.h"
#include <stdexcept>

UndoManager::UndoManager() : isUndoingOrRedoing(false) {}

UndoManager::~UndoManager() {}

void UndoManager::recordAction(const EditorAction& action) {
    if (isUndoingOrRedoing) return;

    undoStack.push(action);
    redoStack.clear();
}

bool UndoManager::canUndo() const {
    return !undoStack.isEmpty();
}

bool UndoManager::canRedo() const {
    return !redoStack.isEmpty();
}

EditorAction UndoManager::undo() {
    if (!canUndo()) {
        throw std::underflow_error("Nothing to undo");
    }

    EditorAction action = undoStack.pop();
    redoStack.push(action);
    return action;
}

EditorAction UndoManager::redo() {
    if (!canRedo()) {
        throw std::underflow_error("Nothing to redo");
    }

    EditorAction action = redoStack.pop();
    undoStack.push(action);
    return action;
}

void UndoManager::clear() {
    undoStack.clear();
    redoStack.clear();
    isUndoingOrRedoing = false;
}
