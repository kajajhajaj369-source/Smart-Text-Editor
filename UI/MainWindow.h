#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QLabel>
#include <QMenu>
#include <QAction>
#include <QCompleter>
#include <QStringListModel>
#include <QFrame>

#include "../Editor/Document.h"
#include "../Editor/UndoManager.h"
#include "../Services/FileManager.h"
#include "../Services/Autocomplete.h"
#include "../Services/DsaLogger.h"
#include "FindReplaceDialog.h"
#include "DsaGuideDialog.h"
#include "DsaActivityDialog.h"

/**
 * @brief Windows Notepad-style Main Window for Smart Text Editor.
 * 
 * Clean, lightweight Notepad UX on the surface with an in-app
 * educational Data Structures laboratory underneath.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    // File operations
    void newFile();
    void openFile();
    bool saveFile();
    bool saveAs();
    void openRecentFile();
    void closeDocument();

    // Edit operations
    void performUndo();
    void performRedo();
    void performDelete();

    // Format operations
    void toggleWordWrap(bool enabled);
    void selectFont();

    // Search operations
    void showFindDialog();
    void showReplaceDialog();
    void findNext(const QString& query, bool matchCase);
    void findPrevious(const QString& query, bool matchCase);
    void replaceOne(const QString& findQuery, const QString& replaceWith, bool matchCase);
    void replaceAll(const QString& findQuery, const QString& replaceWith, bool matchCase);

    // Tools & Educational Views
    void showDsaGuide();
    void showDsaActivity();
    void showStatistics();
    void checkBracketBalance();
    void toggleLiveDsaMapping(bool enabled);
    void showAbout();
    void showProjectInfo();

    // Themes
    void setLightTheme();
    void setDarkTheme();

    // Editor events
    void onTextChanged();
    void onCursorPositionChanged();

    // DSA Logger notification
    void onDsaEventLogged(const DsaEvent& evt);

private:
    // Core DSA Models & Services
    Document document;
    UndoManager undoManager;
    FileManager fileManager;
    Autocomplete autocomplete;

    // UI Widgets
    QPlainTextEdit* editor;
    QLabel* statusCursorPos;
    QLabel* statusWordCount;
    QLabel* statusCharCount;

    // Live Mapping HUD Widget (Floating / Collapsible bar)
    QFrame* liveMappingBar;
    QLabel* liveMappingLabel;
    QLabel* liveCodeTcLabel;
    bool liveMappingEnabled;

    // Menus
    QMenu* fileMenu;
    QMenu* recentFilesMenu;
    QMenu* editMenu;
    QMenu* formatMenu;
    QMenu* viewMenu;
    QMenu* helpMenu;

    // Actions
    QAction* undoAction;
    QAction* redoAction;
    QAction* saveAction;
    QAction* saveAsAction;
    QAction* wordWrapAction;
    QAction* statusBarAction;
    QAction* liveMappingAction;

    // Autocomplete
    QCompleter* completer;
    QStringListModel* completerModel;

    // Dialogs
    FindReplaceDialog* findReplaceDialog;
    DsaGuideDialog* dsaGuideDialog;
    DsaActivityDialog* dsaActivityDialog;

    // Search state
    QString lastSearchQuery;
    bool lastSearchCase;
    int currentMatchIndex;
    int totalMatchesCount;

    // Helper functions
    void createActions();
    void createMenus();
    void createStatusBar();
    void createLiveMappingHUD();
    void setupEditor();
    void setupCompleter();
    void updateRecentFilesMenu();
    void updateStatusBar();
    void highlightBracketMatch();
    bool maybeSave();
    void syncDocumentFromEditor();
    void syncEditorFromDocument();
    QString wordUnderCursor() const;
};

#endif // MAINWINDOW_H
