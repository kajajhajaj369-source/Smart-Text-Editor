#ifndef EDITOR_ACTION_H
#define EDITOR_ACTION_H

#include <string>

enum class ActionType {
    INSERT_TEXT,
    DELETE_TEXT,
    REPLACE_TEXT
};

struct EditorAction {
    ActionType type;
    int position;
    std::string text;
    std::string oldText;

    EditorAction() : type(ActionType::INSERT_TEXT), position(0), text(""), oldText("") {}

    EditorAction(ActionType t, int pos, const std::string& txt, const std::string& oldTxt = "")
        : type(t), position(pos), text(txt), oldText(oldTxt) {}
};

#endif // EDITOR_ACTION_H
