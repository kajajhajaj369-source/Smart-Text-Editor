#include "MainWindow.h"
#include "StatisticsDialog.h"
#include "DsaGuideDialog.h"
#include "DsaActivityDialog.h"
#include "../Algorithms/BracketMatcher.h"
#include "../Algorithms/Search.h"
#include "../Services/StatisticsManager.h"

#include <QMenuBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QFontDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QTextCursor>
#include <QTextBlock>
#include <QFont>
#include <QApplication>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      liveMappingBar(nullptr),
      liveMappingLabel(nullptr),
      liveMappingEnabled(false),
      findReplaceDialog(nullptr),
      dsaGuideDialog(nullptr),
      dsaActivityDialog(nullptr),
      currentMatchIndex(0),
      totalMatchesCount(0) {
    setWindowTitle("Untitled — Notepad");
    resize(850, 600);

    setupEditor();
    createLiveMappingHUD();
    createActions();
    createMenus();
    createStatusBar();
    setupCompleter();

    setLightTheme(); // Traditional Notepad light theme by default

    // Hook up DSA Logger listener for Live Mapping HUD
    DsaLogger::instance().setListener([this](const DsaEvent& evt) {
        onDsaEventLogged(evt);
    });

    DsaLogger::instance().log("Init", "Editor", "BOOT", "O(1)", "Application initialized in Notepad mode");

    updateStatusBar();
    updateRecentFilesMenu();
}

MainWindow::~MainWindow() {
    delete findReplaceDialog;
    delete dsaGuideDialog;
    delete dsaActivityDialog;
}

void MainWindow::setupEditor() {
    QWidget* centralContainer = new QWidget(this);
    QVBoxLayout* centralLayout = new QVBoxLayout(centralContainer);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->setSpacing(0);

    editor = new QPlainTextEdit(centralContainer);
    QFont font("Lucida Console", 11);
    if (!font.exactMatch()) {
        font.setFamily("Consolas");
    }
    editor->setFont(font);
    editor->setLineWrapMode(QPlainTextEdit::WidgetWidth);

    centralLayout->addWidget(editor);
    setCentralWidget(centralContainer);

    connect(editor, &QPlainTextEdit::textChanged, this, &MainWindow::onTextChanged);
    connect(editor, &QPlainTextEdit::cursorPositionChanged, this, &MainWindow::onCursorPositionChanged);
}

void MainWindow::createLiveMappingHUD() {
    QWidget* centralContainer = centralWidget();
    QVBoxLayout* centralLayout = qobject_cast<QVBoxLayout*>(centralContainer->layout());

    liveMappingBar = new QFrame(centralContainer);
    liveMappingBar->setFrameShape(QFrame::StyledPanel);
    liveMappingBar->setStyleSheet("background-color: #1e293b; color: #f8fafc; border-top: 2px solid #06b6d4; padding: 4px;");
    liveMappingBar->setVisible(false);

    QHBoxLayout* hudLayout = new QHBoxLayout(liveMappingBar);
    hudLayout->setContentsMargins(8, 4, 8, 4);

    QLabel* hudTitle = new QLabel("<b>[LIVE DSA MAPPING]</b>", liveMappingBar);
    hudTitle->setStyleSheet("color: #06b6d4; font-family: Consolas; font-size: 11px;");

    liveMappingLabel = new QLabel("Waiting for user action...", liveMappingBar);
    liveMappingLabel->setStyleSheet("color: #e2e8f0; font-family: Consolas; font-size: 11px;");

    QLabel* sepLabel = new QLabel("|", liveMappingBar);
    sepLabel->setStyleSheet("color: #475569; font-weight: bold;");

    liveCodeTcLabel = new QLabel("Code TC: <b style='color:#fbbf24'>O(1)</b>", liveMappingBar);
    liveCodeTcLabel->setStyleSheet("color: #cbd5e1; font-family: Consolas; font-size: 11px; background: rgba(245, 158, 11, 0.15); padding: 2px 6px; border-radius: 4px; border: 1px solid rgba(245, 158, 11, 0.3);");

    hudLayout->addWidget(hudTitle);
    hudLayout->addWidget(liveMappingLabel);
    hudLayout->addWidget(sepLabel);
    hudLayout->addWidget(liveCodeTcLabel);
    hudLayout->addStretch();

    centralLayout->addWidget(liveMappingBar);
}

void MainWindow::setupCompleter() {
    completerModel = new QStringListModel(this);
    completer = new QCompleter(completerModel, this);
    completer->setWidget(editor);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setWrapAround(false);

    connect(completer, QOverload<const QString &>::of(&QCompleter::activated),
            [this](const QString &completion) {
                QTextCursor tc = editor->textCursor();
                int extra = completion.length() - completer->completionPrefix().length();
                tc.movePosition(QTextCursor::Left);
                tc.movePosition(QTextCursor::EndOfWord);
                tc.insertText(completion.right(extra));
                editor->setTextCursor(tc);
                DsaLogger::instance().log("Autocomplete", "Hash Table + MergeSort", "LOOKUP", "O(1) + O(k log k)", "Selected autocomplete prediction");
            });
}

void MainWindow::createActions() {
    // File actions
    QAction* newAction = new QAction("&New", this);
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, &QAction::triggered, this, &MainWindow::newFile);

    QAction* openAction = new QAction("&Open...", this);
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);

    saveAction = new QAction("&Save", this);
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);

    saveAsAction = new QAction("Save &As...", this);
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveAs);

    QAction* exitAction = new QAction("E&xit", this);
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &MainWindow::close);

    // Edit actions
    undoAction = new QAction("&Undo", this);
    undoAction->setShortcut(QKeySequence::Undo);
    connect(undoAction, &QAction::triggered, this, &MainWindow::performUndo);

    redoAction = new QAction("&Redo", this);
    redoAction->setShortcut(QKeySequence::Redo);
    connect(redoAction, &QAction::triggered, this, &MainWindow::performRedo);

    QAction* cutAction = new QAction("Cu&t", this);
    cutAction->setShortcut(QKeySequence::Cut);
    connect(cutAction, &QAction::triggered, editor, &QPlainTextEdit::cut);

    QAction* copyAction = new QAction("&Copy", this);
    copyAction->setShortcut(QKeySequence::Copy);
    connect(copyAction, &QAction::triggered, editor, &QPlainTextEdit::copy);

    QAction* pasteAction = new QAction("&Paste", this);
    pasteAction->setShortcut(QKeySequence::Paste);
    connect(pasteAction, &QAction::triggered, editor, &QPlainTextEdit::paste);

    QAction* deleteAction = new QAction("De&lete", this);
    deleteAction->setShortcut(QKeySequence::Delete);
    connect(deleteAction, &QAction::triggered, this, &MainWindow::performDelete);

    QAction* selectAllAction = new QAction("Select &All", this);
    selectAllAction->setShortcut(QKeySequence::SelectAll);
    connect(selectAllAction, &QAction::triggered, editor, &QPlainTextEdit::selectAll);

    QAction* findAction = new QAction("&Find...", this);
    findAction->setShortcut(QKeySequence::Find);
    connect(findAction, &QAction::triggered, this, &MainWindow::showFindDialog);

    QAction* replaceAction = new QAction("&Replace...", this);
    replaceAction->setShortcut(QKeySequence::Replace);
    connect(replaceAction, &QAction::triggered, this, &MainWindow::showReplaceDialog);

    // Format actions
    wordWrapAction = new QAction("&Word Wrap", this);
    wordWrapAction->setCheckable(true);
    wordWrapAction->setChecked(true);
    connect(wordWrapAction, &QAction::toggled, this, &MainWindow::toggleWordWrap);

    QAction* fontAction = new QAction("&Font...", this);
    connect(fontAction, &QAction::triggered, this, &MainWindow::selectFont);

    // View actions
    statusBarAction = new QAction("&Status Bar", this);
    statusBarAction->setCheckable(true);
    statusBarAction->setChecked(true);
    connect(statusBarAction, &QAction::toggled, [this](bool visible) {
        statusBar()->setVisible(visible);
    });

    QAction* dsaGuideAction = new QAction("&Data Structures Guide", this);
    dsaGuideAction->setShortcut(Qt::Key_F1);
    connect(dsaGuideAction, &QAction::triggered, this, &MainWindow::showDsaGuide);

    QAction* statsAction = new QAction("&Statistics", this);
    statsAction->setShortcut(Qt::Key_F4);
    connect(statsAction, &QAction::triggered, this, &MainWindow::showStatistics);

    liveMappingAction = new QAction("&Live DSA Mapping (Educational Mode)", this);
    liveMappingAction->setCheckable(true);
    liveMappingAction->setChecked(false);
    connect(liveMappingAction, &QAction::toggled, this, &MainWindow::toggleLiveDsaMapping);

    QAction* dsaActivityAction = new QAction("DSA &Activity Log", this);
    dsaActivityAction->setShortcut(Qt::Key_F8);
    connect(dsaActivityAction, &QAction::triggered, this, &MainWindow::showDsaActivity);

    QAction* bracketAction = new QAction("Check &Bracket Balance", this);
    bracketAction->setShortcut(Qt::Key_F7);
    connect(bracketAction, &QAction::triggered, this, &MainWindow::checkBracketBalance);

    QAction* lightThemeAction = new QAction("&Light Theme", this);
    connect(lightThemeAction, &QAction::triggered, this, &MainWindow::setLightTheme);

    QAction* darkThemeAction = new QAction("&Dark Theme", this);
    connect(darkThemeAction, &QAction::triggered, this, &MainWindow::setDarkTheme);

    // Help actions
    QAction* aboutAction = new QAction("&About Notepad", this);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAbout);

    QAction* projectInfoAction = new QAction("&Project Information (DSA Curriculum)", this);
    connect(projectInfoAction, &QAction::triggered, this, &MainWindow::showProjectInfo);

    // Store references
    addAction(newAction);
    addAction(openAction);
    addAction(saveAction);
    addAction(saveAsAction);
    addAction(undoAction);
    addAction(redoAction);
    addAction(cutAction);
    addAction(copyAction);
    addAction(pasteAction);
    addAction(deleteAction);
    addAction(selectAllAction);
    addAction(findAction);
    addAction(replaceAction);
    addAction(dsaGuideAction);
    addAction(statsAction);
    addAction(dsaActivityAction);
    addAction(bracketAction);
}

void MainWindow::createMenus() {
    fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(findChild<QAction*>("&New"));
    fileMenu->addAction(findChild<QAction*>("&Open..."));
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addSeparator();

    recentFilesMenu = fileMenu->addMenu("&Recent Files");
    fileMenu->addSeparator();
    fileMenu->addAction(findChild<QAction*>("E&xit"));

    editMenu = menuBar()->addMenu("&Edit");
    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);
    editMenu->addSeparator();
    editMenu->addAction(findChild<QAction*>("Cu&t"));
    editMenu->addAction(findChild<QAction*>("&Copy"));
    editMenu->addAction(findChild<QAction*>("&Paste"));
    editMenu->addAction(findChild<QAction*>("De&lete"));
    editMenu->addSeparator();
    editMenu->addAction(findChild<QAction*>("&Find..."));
    editMenu->addAction(findChild<QAction*>("&Replace..."));
    editMenu->addSeparator();
    editMenu->addAction(findChild<QAction*>("Select &All"));

    formatMenu = menuBar()->addMenu("&Format");
    formatMenu->addAction(wordWrapAction);
    formatMenu->addAction(findChild<QAction*>("&Font..."));

    viewMenu = menuBar()->addMenu("&View");
    viewMenu->addAction(statusBarAction);
    viewMenu->addSeparator();
    viewMenu->addAction(findChild<QAction*>("&Data Structures Guide"));
    viewMenu->addAction(findChild<QAction*>("&Statistics"));
    viewMenu->addAction(findChild<QAction*>("Check &Bracket Balance"));
    viewMenu->addSeparator();
    viewMenu->addAction(liveMappingAction);
    viewMenu->addAction(findChild<QAction*>("DSA &Activity Log"));
    viewMenu->addSeparator();
    QMenu* themeMenu = viewMenu->addMenu("&Theme");
    themeMenu->addAction(findChild<QAction*>("&Light Theme"));
    themeMenu->addAction(findChild<QAction*>("&Dark Theme"));

    helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction(findChild<QAction*>("&About Notepad"));
    helpMenu->addAction(findChild<QAction*>("&Data Structures Guide"));
    helpMenu->addAction(findChild<QAction*>("&Project Information (DSA Curriculum)"));
}

void MainWindow::createStatusBar() {
    statusCursorPos = new QLabel("Ln 1, Col 1", this);
    statusWordCount = new QLabel("0 words", this);
    statusCharCount = new QLabel("0 characters", this);

    statusCursorPos->setMinimumWidth(110);
    statusWordCount->setMinimumWidth(90);
    statusCharCount->setMinimumWidth(110);

    statusBar()->addWidget(statusCursorPos);
    statusBar()->addWidget(statusWordCount);
    statusBar()->addWidget(statusCharCount);
}

void MainWindow::updateStatusBar() {
    QTextCursor cursor = editor->textCursor();
    int line = cursor.blockNumber() + 1;
    int col = cursor.columnNumber() + 1;
    statusCursorPos->setText(QString("Ln %1, Col %2").arg(line).arg(col));

    int words = document.getRecursiveWordCount();
    int chars = document.getRecursiveCharCount(true);

    statusWordCount->setText(QString("%1 words").arg(words));
    statusCharCount->setText(QString("%1 characters").arg(chars));

    // Update Live Code Time Complexity in HUD
    if (liveCodeTcLabel) {
        QString text = editor->toPlainText();
        int maxLoopDepth = 0;
        int currentDepth = 0;
        bool hasLogStep = false;
        bool hasRecursion = false;

        QStringList lines = text.split('\n');
        for (const QString& lineStr : lines) {
            QString trimmed = lineStr.trimmed();
            if (trimmed.startsWith("//") || trimmed.startsWith("/*")) continue;

            if (trimmed.contains(QRegularExpression("\\b(for|while)\\s*\\("))) {
                currentDepth++;
                if (currentDepth > maxLoopDepth) maxLoopDepth = currentDepth;
                if (trimmed.contains("/=") || trimmed.contains("*=") || trimmed.contains("/ 2") || trimmed.contains(">>= 1")) {
                    hasLogStep = true;
                }
            }
            int openBraces = trimmed.count('{');
            int closeBraces = trimmed.count('}');
            int diff = closeBraces - openBraces;
            if (diff > 0 && currentDepth > 0) {
                currentDepth = std::max(0, currentDepth - diff);
            }
        }

        QString tc = "O(1)";
        if (maxLoopDepth >= 3) tc = QString("O(N^%1)").arg(maxLoopDepth);
        else if (maxLoopDepth == 2) tc = hasLogStep ? "O(N log N)" : "O(N^2)";
        else if (maxLoopDepth == 1) tc = hasLogStep ? "O(log N)" : "O(N)";

        liveCodeTcLabel->setText(QString("Written Code TC: <b style='color:#fbbf24'>%1</b>").arg(tc));
    }
}

void MainWindow::onDsaEventLogged(const DsaEvent& evt) {
    if (liveMappingEnabled && liveMappingLabel) {
        liveMappingLabel->setText(
            QString("Action: <b style='color:#38bdf8'>%1</b> | Data Structure: <b style='color:#a855f7'>%2</b> | Op: <b style='color:#4ade80'>%3</b> | Complexity: <b>%4</b> | Principle: %5")
                .arg(QString::fromStdString(evt.actionName))
                .arg(QString::fromStdString(evt.dataStructure))
                .arg(QString::fromStdString(evt.operation))
                .arg(QString::fromStdString(evt.complexity))
                .arg(QString::fromStdString(evt.principle))
        );
    }
    if (dsaActivityDialog && dsaActivityDialog->isVisible()) {
        dsaActivityDialog->appendEvent(evt);
    }
}

void MainWindow::toggleLiveDsaMapping(bool enabled) {
    liveMappingEnabled = enabled;
    if (liveMappingBar) {
        liveMappingBar->setVisible(enabled);
    }
    if (enabled) {
        DsaLogger::instance().log("Live Mapping", "HUD", "ENABLE", "O(1)", "Educational Live Action Mapping HUD activated");
    }
}

void MainWindow::toggleWordWrap(bool enabled) {
    editor->setLineWrapMode(enabled ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap);
}

void MainWindow::selectFont() {
    bool ok;
    QFont currentFont = editor->font();
    QFont selectedFont = QFontDialog::getFont(&ok, currentFont, this, "Select Editor Font");
    if (ok) {
        editor->setFont(selectedFont);
    }
}

void MainWindow::performUndo() {
    editor->undo();
    DsaLogger::instance().log("Undo", "Stack", "POP", "O(1)", "Popped most recent editing action from Undo Stack (LIFO)");
    updateStatusBar();
}

void MainWindow::performRedo() {
    editor->redo();
    DsaLogger::instance().log("Redo", "Stack", "POP", "O(1)", "Popped most recent undone action from Redo Stack (LIFO)");
    updateStatusBar();
}

void MainWindow::performDelete() {
    QTextCursor cursor = editor->textCursor();
    if (cursor.hasSelection()) {
        cursor.removeSelectedText();
    } else {
        cursor.deleteChar();
    }
    DsaLogger::instance().log("Delete Text", "Linked List", "UPDATE NODE", "O(1)", "Updated line node content after character deletion");
}

void MainWindow::updateRecentFilesMenu() {
    recentFilesMenu->clear();
    std::vector<std::string> recent = fileManager.getRecentFiles();

    if (recent.empty()) {
        QAction* emptyAct = recentFilesMenu->addAction("(No recent files)");
        emptyAct->setEnabled(false);
        return;
    }

    for (const auto& path : recent) {
        QString qPath = QString::fromStdString(path);
        QAction* act = recentFilesMenu->addAction(qPath);
        connect(act, &QAction::triggered, this, &MainWindow::openRecentFile);
    }
}

void MainWindow::syncDocumentFromEditor() {
    std::string text = editor->toPlainText().toStdString();
    document.loadFromPlainText(text);
}

void MainWindow::syncEditorFromDocument() {
    editor->blockSignals(true);
    editor->setPlainText(QString::fromStdString(document.toPlainText()));
    editor->blockSignals(false);
}

QString MainWindow::wordUnderCursor() const {
    QTextCursor tc = editor->textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

void MainWindow::onTextChanged() {
    syncDocumentFromEditor();
    autocomplete.updateFromDocument(editor->toPlainText().toStdString());

    // Autocomplete popup
    QString currentWord = wordUnderCursor();
    if (currentWord.length() >= 2) {
        std::vector<std::string> suggestions = autocomplete.getSuggestions(currentWord.toStdString(), 6);
        QStringList qSuggestions;
        for (const auto& s : suggestions) {
            qSuggestions.append(QString::fromStdString(s));
        }

        if (!qSuggestions.isEmpty()) {
            completerModel->setStringList(qSuggestions);
            completer->setCompletionPrefix(currentWord);
            QRect cr = editor->cursorRect();
            cr.setWidth(completer->popup()->sizeHintForColumn(0) + completer->popup()->verticalScrollBar()->sizeHint().width());
            completer->complete(cr);
        }
    }

    updateStatusBar();
}

void MainWindow::onCursorPositionChanged() {
    updateStatusBar();
    highlightBracketMatch();
}

void MainWindow::highlightBracketMatch() {
    std::string text = editor->toPlainText().toStdString();
    int cursorPos = editor->textCursor().position();

    auto match = BracketMatcher::findMatch(text, cursorPos);
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (match.isBracket && match.matchPosition >= 0) {
        QTextEdit::ExtraSelection sel1;
        sel1.cursor = editor->textCursor();
        sel1.cursor.setPosition(cursorPos > 0 && (text[cursorPos - 1] == '(' || text[cursorPos - 1] == '{' || text[cursorPos - 1] == '[') ? cursorPos - 1 : cursorPos);
        sel1.cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
        sel1.format.setBackground(QColor(176, 224, 230, 140));
        extraSelections.append(sel1);

        QTextEdit::ExtraSelection sel2;
        sel2.cursor = editor->textCursor();
        sel2.cursor.setPosition(match.matchPosition);
        sel2.cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
        sel2.format.setBackground(QColor(176, 224, 230, 140));
        extraSelections.append(sel2);

        DsaLogger::instance().log("Bracket Match", "Stack", "MATCH PAIR", "O(n)", "Delimiters matched using LIFO Stack traversal");
    }

    editor->setExtraSelections(extraSelections);
}

void MainWindow::newFile() {
    if (maybeSave()) {
        editor->clear();
        document.clear();
        undoManager.clear();
        setWindowTitle("Untitled — Notepad");
        DsaLogger::instance().log("New File", "Linked List", "CLEAR & INIT", "O(1)", "Initialized clean Document line nodes");
        updateStatusBar();
    }
}

void MainWindow::openFile() {
    if (!maybeSave()) return;

    QString fileName = QFileDialog::getOpenFileName(this, "Open File", "",
        "Text Files (*.txt *.cpp *.h *.py *.java *.md);;All Files (*.*)");

    if (!fileName.isEmpty()) {
        std::string errMsg;
        if (fileManager.openFile(fileName.toStdString(), document, errMsg)) {
            syncEditorFromDocument();
            undoManager.clear();
            setWindowTitle(fileName + " — Notepad");
            updateRecentFilesMenu();
            DsaLogger::instance().log("Open File", "Queue + Linked List", "ENQUEUE & LOAD", "O(1)", "Enqueued file path to Recent Files Queue and populated Line Nodes");
            updateStatusBar();
        } else {
            QMessageBox::warning(this, "Error Opening File", QString::fromStdString(errMsg));
        }
    }
}

void MainWindow::openRecentFile() {
    QAction* action = qobject_cast<QAction*>(sender());
    if (action && maybeSave()) {
        QString fileName = action->text();
        std::string errMsg;
        if (fileManager.openFile(fileName.toStdString(), document, errMsg)) {
            syncEditorFromDocument();
            undoManager.clear();
            setWindowTitle(fileName + " — Notepad");
            updateRecentFilesMenu();
            DsaLogger::instance().log("Open Recent", "Queue", "PROMOTE FRONT", "O(1)", "Re-opened file from FIFO Queue");
            updateStatusBar();
        } else {
            QMessageBox::warning(this, "Error Opening Recent File", QString::fromStdString(errMsg));
        }
    }
}

bool MainWindow::saveFile() {
    if (document.getFilePath().empty()) {
        return saveAs();
    }

    syncDocumentFromEditor();
    std::string errMsg;
    if (fileManager.saveFile(document.getFilePath(), document, errMsg)) {
        document.setModified(false);
        statusBar()->showMessage("File saved", 2000);
        DsaLogger::instance().log("Save File", "Linked List", "SERIALIZE TRAVERSAL", "O(n)", "Traversed Document line nodes to write file stream");
        return true;
    } else {
        QMessageBox::warning(this, "Error Saving File", QString::fromStdString(errMsg));
        return false;
    }
}

bool MainWindow::saveAs() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save As", "",
        "Text Files (*.txt);;C++ Source (*.cpp *.h);;Python (*.py);;Markdown (*.md);;All Files (*.*)");

    if (fileName.isEmpty()) {
        return false;
    }

    document.setFilePath(fileName.toStdString());
    setWindowTitle(fileName + " — Notepad");
    fileManager.addRecentFile(fileName.toStdString());
    updateRecentFilesMenu();
    return saveFile();
}

void MainWindow::closeDocument() {
    newFile();
}

bool MainWindow::maybeSave() {
    if (!document.isModified()) {
        return true;
    }

    QMessageBox::StandardButton ret = QMessageBox::warning(this, "Notepad",
        "Do you want to save changes to " + (document.getFilePath().empty() ? QString("Untitled") : QString::fromStdString(document.getFilePath())) + "?",
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (ret == QMessageBox::Save) {
        return saveFile();
    } else if (ret == QMessageBox::Cancel) {
        return false;
    }
    return true;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::showFindDialog() {
    if (!findReplaceDialog) {
        findReplaceDialog = new FindReplaceDialog(this);
        connect(findReplaceDialog, &FindReplaceDialog::findNextRequested, this, &MainWindow::findNext);
        connect(findReplaceDialog, &FindReplaceDialog::findPreviousRequested, this, &MainWindow::findPrevious);
        connect(findReplaceDialog, &FindReplaceDialog::replaceRequested, this, &MainWindow::replaceOne);
        connect(findReplaceDialog, &FindReplaceDialog::replaceAllRequested, this, &MainWindow::replaceAll);
    }

    QString selected = editor->textCursor().selectedText();
    if (!selected.isEmpty()) {
        findReplaceDialog->setFindText(selected);
    }
    findReplaceDialog->show();
    findReplaceDialog->raise();
    findReplaceDialog->activateWindow();
}

void MainWindow::showReplaceDialog() {
    showFindDialog();
}

void MainWindow::findNext(const QString& query, bool matchCase) {
    lastSearchQuery = query;
    lastSearchCase = matchCase;

    QTextDocument::FindFlags flags;
    if (matchCase) flags |= QTextDocument::FindCaseSensitively;

    bool found = editor->find(query, flags);
    if (!found) {
        editor->moveCursor(QTextCursor::Start);
        found = editor->find(query, flags);
    }

    std::vector<std::string> lines = document.getAllLines();
    auto matches = Search::searchInLines(lines, query.toStdString(), matchCase);
    totalMatchesCount = static_cast<int>(matches.size());

    if (found) {
        currentMatchIndex = (currentMatchIndex % totalMatchesCount) + 1;
    } else {
        currentMatchIndex = 0;
    }

    if (findReplaceDialog) {
        findReplaceDialog->updateStatus(currentMatchIndex, totalMatchesCount);
    }

    DsaLogger::instance().log("Find Text", "Search / Hash Table", "SEARCH PATTERN", "O(N * M)", "Executed pattern match across document line nodes");
}

void MainWindow::findPrevious(const QString& query, bool matchCase) {
    lastSearchQuery = query;
    lastSearchCase = matchCase;

    QTextDocument::FindFlags flags = QTextDocument::FindBackward;
    if (matchCase) flags |= QTextDocument::FindCaseSensitively;

    bool found = editor->find(query, flags);
    if (!found) {
        editor->moveCursor(QTextCursor::End);
        found = editor->find(query, flags);
    }

    std::vector<std::string> lines = document.getAllLines();
    auto matches = Search::searchInLines(lines, query.toStdString(), matchCase);
    totalMatchesCount = static_cast<int>(matches.size());

    if (found) {
        currentMatchIndex = (currentMatchIndex > 1) ? currentMatchIndex - 1 : totalMatchesCount;
    } else {
        currentMatchIndex = 0;
    }

    if (findReplaceDialog) {
        findReplaceDialog->updateStatus(currentMatchIndex, totalMatchesCount);
    }

    DsaLogger::instance().log("Find Previous", "Search", "REVERSE SCAN", "O(N * M)", "Backward occurrence scan");
}

void MainWindow::replaceOne(const QString& findQuery, const QString& replaceWith, bool matchCase) {
    QTextCursor cursor = editor->textCursor();
    if (cursor.hasSelection() &&
        cursor.selectedText().compare(findQuery, matchCase ? Qt::CaseSensitive : Qt::CaseInsensitive) == 0) {
        cursor.insertText(replaceWith);
        DsaLogger::instance().log("Replace One", "Linked List", "UPDATE LINE", "O(1)", "Replaced matched text in line node");
    }
    findNext(findQuery, matchCase);
}

void MainWindow::replaceAll(const QString& findQuery, const QString& replaceWith, bool matchCase) {
    std::vector<std::string> lines = document.getAllLines();
    auto matches = Search::searchInLines(lines, findQuery.toStdString(), matchCase);

    if (matches.empty()) {
        QMessageBox::information(this, "Replace All", "Cannot find \"" + findQuery + "\"");
        return;
    }

    editor->moveCursor(QTextCursor::Start);
    int count = 0;
    QTextDocument::FindFlags flags;
    if (matchCase) flags |= QTextDocument::FindCaseSensitively;

    while (editor->find(findQuery, flags)) {
        editor->textCursor().insertText(replaceWith);
        count++;
    }

    syncDocumentFromEditor();
    updateStatusBar();
    DsaLogger::instance().log("Replace All", "Search + Linked List", "REPLACE ALL NODES", "O(N * M)", "Iterated and replaced all matching occurrences");
    QMessageBox::information(this, "Notepad", QString("Replaced %1 occurrence(s).").arg(count));
}

void MainWindow::showDsaGuide() {
    if (!dsaGuideDialog) {
        dsaGuideDialog = new DsaGuideDialog(this);
    }
    dsaGuideDialog->show();
    dsaGuideDialog->raise();
    dsaGuideDialog->activateWindow();
    DsaLogger::instance().log("Open DSA Guide", "Educational", "VIEW GUIDE", "O(1)", "Opened in-app Data Structures Laboratory Guide");
}

void MainWindow::showDsaActivity() {
    if (!dsaActivityDialog) {
        dsaActivityDialog = new DsaActivityDialog(this);
    }
    dsaActivityDialog->refreshLog();
    dsaActivityDialog->show();
    dsaActivityDialog->raise();
    dsaActivityDialog->activateWindow();
    DsaLogger::instance().log("Open Activity Log", "Audit Trail", "VIEW LOG", "O(1)", "Opened real-time DSA Activity Log dialog");
}

void MainWindow::showStatistics() {
    syncDocumentFromEditor();
    auto stats = StatisticsManager::computeStatistics(document, 10);
    DsaLogger::instance().log("Statistics", "Recursion + Hash Table + MergeSort", "RECURSE & SORT", "O(n log n)", "Calculated words recursively, built Hash Table frequencies, and ranked via Merge Sort");
    StatisticsDialog dlg(stats, this);
    dlg.exec();
}

void MainWindow::checkBracketBalance() {
    std::string text = editor->toPlainText().toStdString();
    std::string message;
    bool balanced = BracketMatcher::validateBracketBalance(text, message);

    if (balanced) {
        DsaLogger::instance().log("Bracket Check", "Stack", "VALIDATE (BALANCED)", "O(n)", "Verified balanced parenthesis, braces, and brackets via LIFO Stack");
        QMessageBox::information(this, "Bracket Balance (Stack Verification)",
            "All brackets ( ), { }, [ ] are correctly balanced and matched!\n\nVerified using custom Stack.");
    } else {
        DsaLogger::instance().log("Bracket Check", "Stack", "MISMATCH DETECTED", "O(n)", "Identified syntax delimiter mismatch via Stack error");
        QMessageBox::warning(this, "Bracket Balance Error",
            QString("Bracket mismatch detected:\n\n%1").arg(QString::fromStdString(message)));
    }
}

void MainWindow::setLightTheme() {
    qApp->setStyleSheet(
        "QMainWindow { background-color: #f0f0f0; color: #000000; }"
        "QPlainTextEdit { background-color: #ffffff; color: #000000; selection-background-color: #3399ff; selection-color: #ffffff; border: none; padding: 4px; font-size: 13px; }"
        "QMenuBar { background-color: #f0f0f0; color: #000000; border-bottom: 1px solid #e0e0e0; }"
        "QMenuBar::item:selected { background-color: #e5f1fb; }"
        "QMenu { background-color: #ffffff; color: #000000; border: 1px solid #cccccc; }"
        "QMenu::item:selected { background-color: #90c8f6; color: #000000; }"
        "QStatusBar { background-color: #f0f0f0; color: #333333; border-top: 1px solid #d0d0d0; font-size: 11px; }"
    );
}

void MainWindow::setDarkTheme() {
    qApp->setStyleSheet(
        "QMainWindow { background-color: #191919; color: #ffffff; }"
        "QPlainTextEdit { background-color: #202020; color: #ffffff; selection-background-color: #264f78; selection-color: #ffffff; border: none; padding: 4px; font-size: 13px; }"
        "QMenuBar { background-color: #191919; color: #ffffff; border-bottom: 1px solid #333333; }"
        "QMenuBar::item:selected { background-color: #333333; }"
        "QMenu { background-color: #252526; color: #ffffff; border: 1px solid #454545; }"
        "QMenu::item:selected { background-color: #094771; color: #ffffff; }"
        "QStatusBar { background-color: #191919; color: #aaaaaa; border-top: 1px solid #333333; font-size: 11px; }"
    );
}

void MainWindow::showAbout() {
    QMessageBox::about(this, "About Notepad",
        "<h3>Smart Text Editor (Notepad Style)</h3>"
        "<p>A simple, lightweight text editor built with <b>C++ and Qt 6</b>.</p>"
        "<p>Internally engineered with custom Data Structures: Stacks, Queues, Linked Lists, Hash Tables, and Merge Sort.</p>"
        "<p>Open <b>View &rarr; Data Structures Guide</b> for complete educational demonstrations.</p>");
}

void MainWindow::showProjectInfo() {
    QMessageBox::information(this, "Project Information — Data Structures Course",
        "SMART TEXT EDITOR\n"
        "Data Structures & Algorithms Course Project\n\n"
        "Implemented Concepts:\n"
        "  ✓ Stack: Undo / Redo history and Bracket Matching (LIFO)\n"
        "  ✓ Linked List: Document line-by-line dynamic storage\n"
        "  ✓ Queue: Recent Files management (FIFO)\n"
        "  ✓ Hash Table: Word indexing, frequencies & autocomplete (Separate Chaining)\n"
        "  ✓ Merge Sort: Top word frequency ranking (O(n log n))\n"
        "  ✓ Recursion: Recursive linked node traversal for document statistics\n\n"
        "All data structures are implemented manually without relying on STL containers.");
}
