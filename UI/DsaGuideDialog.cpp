#include "DsaGuideDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QScrollArea>
#include <QFont>
#include <vector>

DsaGuideDialog::DsaGuideDialog(QWidget *parent)
    : QDialog(parent),
      sandboxQueue(8),
      sandboxHashTable(13) {
    setWindowTitle("Data Structures & Algorithms Laboratory Guide");
    resize(780, 560);
    setupUI();

    // Initialize sandboxes with sample data
    sandboxStack.push("Insert 'Hello'");
    sandboxStack.push("Insert 'World'");
    sandboxStack.push("Delete '!'");
    updateStackVisual();

    sandboxQueue.enqueue("document.txt");
    sandboxQueue.enqueue("main.cpp");
    sandboxQueue.enqueue("report.pdf");
    updateQueueVisual();

    sandboxList.push_back("Line 1: #include <iostream>");
    sandboxList.push_back("Line 2: int main() {");
    sandboxList.push_back("Line 3:     return 0;");
    sandboxList.push_back("Line 4: }");
    updateListVisual();

    sandboxHashTable.incrementFrequency("algorithm", 15);
    sandboxHashTable.incrementFrequency("data", 21);
    sandboxHashTable.incrementFrequency("stack", 8);
    sandboxHashTable.incrementFrequency("queue", 6);
    updateHashVisual();
}

DsaGuideDialog::~DsaGuideDialog() {}

void DsaGuideDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(8);

    tabWidget = new QTabWidget(this);
    tabWidget->addTab(createOverviewTab(), "📋 Overview & Viva Map");
    tabWidget->addTab(createStackTab(), "🥞 Stack (Undo/Redo)");
    tabWidget->addTab(createQueueTab(), "🚶 Queue (Recent Files)");
    tabWidget->addTab(createLinkedListTab(), "🔗 Linked List (Doc Lines)");
    tabWidget->addTab(createHashTableTab(), "⚡ Hash Table (Words)");
    tabWidget->addTab(createMergeSortAndRecursionTab(), "🔀 MergeSort & Recursion");

    mainLayout->addWidget(tabWidget);

    QHBoxLayout* bottomLayout = new QHBoxLayout();
    QLabel* tipLabel = new QLabel("💡 <i>Tip for Viva: Every interactive tab executes on our custom C++ data structures.</i>", this);
    QPushButton* closeBtn = new QPushButton("&Close Guide", this);
    closeBtn->setDefault(true);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);

    bottomLayout->addWidget(tipLabel);
    bottomLayout->addStretch();
    bottomLayout->addWidget(closeBtn);

    mainLayout->addLayout(bottomLayout);
}

QWidget* DsaGuideDialog::createOverviewTab() {
    QWidget* widget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(widget);

    QTextEdit* textEdit = new QTextEdit(widget);
    textEdit->setReadOnly(true);
    textEdit->setHtml(
        "<h2>🎓 Smart Text Editor — Data Structures Architecture</h2>"
        "<p>This application was architected as a dual-layer project: a <b>clean Windows Notepad</b> on top, and an <b>explainable Data Structures laboratory</b> underneath.</p>"
        "<hr/>"
        "<h3>Mapping of Features to Data Structures:</h3>"
        "<table border='1' cellspacing='0' cellpadding='6' width='100%' style='border-collapse: collapse; font-family: Consolas, monospace; font-size: 12px;'>"
        "<tr bgcolor='#f0f0f0'><th>Application Feature</th><th>Data Structure / Algorithm</th><th>Operation</th><th>Time Complexity</th><th>Principle / Rationale</th></tr>"
        "<tr><td><b>Undo / Redo</b></td><td>Custom <code>Stack&lt;T&gt;</code></td><td>PUSH, POP</td><td><b>O(1)</b></td><td>LIFO: Last operation is reversed first</td></tr>"
        "<tr><td><b>Document Lines</b></td><td>Custom <code>LinkedList&lt;T&gt;</code></td><td>INSERT, DELETE, TRAVERSE</td><td><b>O(n)</b></td><td>Non-contiguous dynamic line allocation</td></tr>"
        "<tr><td><b>Recent Files</b></td><td>Custom <code>Queue&lt;T&gt;</code></td><td>ENQUEUE, DEQUEUE</td><td><b>O(1)</b></td><td>FIFO: Oldest entry evicted when full (10)</td></tr>"
        "<tr><td><b>Word Frequency</b></td><td>Custom <code>HashTable&lt;K,V&gt;</code></td><td>INSERT, SEARCH, UPDATE</td><td><b>O(1) avg</b></td><td>Separate chaining + prime polynomial hash</td></tr>"
        "<tr><td><b>Stats Ranking</b></td><td>Custom <code>MergeSort</code></td><td>Divide &rarr; Sort &rarr; Merge</td><td><b>O(n log n)</b></td><td>Guaranteed stable divide-and-conquer</td></tr>"
        "<tr><td><b>Bracket Matching</b></td><td>Custom <code>Stack&lt;T&gt;</code></td><td>PUSH open, POP match</td><td><b>O(n)</b></td><td>Nested delimiter validation <code>()</code> <code>{}</code> <code>[]</code></td></tr>"
        "<tr><td><b>Document Stats</b></td><td><b>Recursion</b></td><td><code>countWords(node-&gt;next)</code></td><td><b>O(lines)</b></td><td>Recursive traversal until base case <code>nullptr</code></td></tr>"
        "</table>"
    );

    layout->addWidget(textEdit);
    return widget;
}

QWidget* DsaGuideDialog::createStackTab() {
    QWidget* widget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(widget);

    QLabel* explLabel = new QLabel(
        "<b>Stack (LIFO — Last In, First Out)</b><br/>"
        "Used for: <b>Undo / Redo History</b> & <b>Bracket Matching</b>.<br/>"
        "<i>Why?</i> The most recent editing operation must be reversed first. When typing, actions are <b>PUSHed</b>. When pressing Ctrl+Z, the top action is <b>POPed</b>.",
        widget
    );
    explLabel->setWordWrap(true);
    layout->addWidget(explLabel);

    QGroupBox* box = new QGroupBox("Interactive Stack Sandbox (Custom Stack<std::string>)", widget);
    QGridLayout* grid = new QGridLayout(box);

    stackInput = new QLineEdit("Type new text...", box);
    QPushButton* pushBtn = new QPushButton("PUSH (Edit Action)", box);
    QPushButton* popBtn = new QPushButton("POP (Undo Action)", box);
    QPushButton* clearBtn = new QPushButton("Clear Stack", box);

    stackVisualList = new QListWidget(box);
    stackVisualList->setFont(QFont("Consolas", 10));
    stackStatusLabel = new QLabel("Stack Size: 0", box);
    stackStatusLabel->setStyleSheet("font-weight: bold; color: #0078d4;");

    grid->addWidget(new QLabel("Action Description:", box), 0, 0);
    grid->addWidget(stackInput, 0, 1);
    grid->addWidget(pushBtn, 0, 2);
    grid->addWidget(popBtn, 0, 3);
    grid->addWidget(clearBtn, 0, 4);
    grid->addWidget(stackVisualList, 1, 0, 1, 5);
    grid->addWidget(stackStatusLabel, 2, 0, 1, 5);

    layout->addWidget(box);

    connect(pushBtn, &QPushButton::clicked, this, &DsaGuideDialog::onStackPushClicked);
    connect(popBtn, &QPushButton::clicked, this, &DsaGuideDialog::onStackPopClicked);
    connect(clearBtn, &QPushButton::clicked, this, &DsaGuideDialog::onStackClearClicked);

    return widget;
}

QWidget* DsaGuideDialog::createQueueTab() {
    QWidget* widget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(widget);

    QLabel* explLabel = new QLabel(
        "<b>Queue (FIFO — First In, First Out)</b><br/>"
        "Used for: <b>Recent Files History</b>.<br/>"
        "<i>Why?</i> Older opened files are removed from the bottom when the list reaches max capacity (10). Opening a file performs <b>ENQUEUE</b>; evicting oldest performs <b>DEQUEUE</b>.",
        widget
    );
    explLabel->setWordWrap(true);
    layout->addWidget(explLabel);

    QGroupBox* box = new QGroupBox("Interactive Queue Sandbox (Custom Circular Queue<std::string>)", widget);
    QGridLayout* grid = new QGridLayout(box);

    queueInput = new QLineEdit("project_notes.txt", box);
    QPushButton* enqBtn = new QPushButton("ENQUEUE (Open File)", box);
    QPushButton* deqBtn = new QPushButton("DEQUEUE (Evict Oldest)", box);
    QPushButton* clearBtn = new QPushButton("Clear Queue", box);

    queueVisualList = new QListWidget(box);
    queueVisualList->setFont(QFont("Consolas", 10));
    queueStatusLabel = new QLabel("Queue Size: 0 / 8", box);
    queueStatusLabel->setStyleSheet("font-weight: bold; color: #107c41;");

    grid->addWidget(new QLabel("Filename:", box), 0, 0);
    grid->addWidget(queueInput, 0, 1);
    grid->addWidget(enqBtn, 0, 2);
    grid->addWidget(deqBtn, 0, 3);
    grid->addWidget(clearBtn, 0, 4);
    grid->addWidget(queueVisualList, 1, 0, 1, 5);
    grid->addWidget(queueStatusLabel, 2, 0, 1, 5);

    layout->addWidget(box);

    connect(enqBtn, &QPushButton::clicked, this, &DsaGuideDialog::onQueueEnqueueClicked);
    connect(deqBtn, &QPushButton::clicked, this, &DsaGuideDialog::onQueueDequeueClicked);
    connect(clearBtn, &QPushButton::clicked, this, &DsaGuideDialog::onQueueClearClicked);

    return widget;
}

QWidget* DsaGuideDialog::createLinkedListTab() {
    QWidget* widget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(widget);

    QLabel* explLabel = new QLabel(
        "<b>Linked List (Doubly-Linked Line Nodes)</b><br/>"
        "Used for: <b>Document Line Representation</b>.<br/>"
        "<i>Why?</i> Represents the document as discrete connected nodes (<code>[Line 1] &harr; [Line 2] &harr; NULL</code>). Inserting/deleting lines does not shift the entire document buffer in memory.",
        widget
    );
    explLabel->setWordWrap(true);
    layout->addWidget(explLabel);

    QGroupBox* box = new QGroupBox("Interactive Linked List Sandbox (Custom LinkedList<std::string>)", widget);
    QGridLayout* grid = new QGridLayout(box);

    listInput = new QLineEdit("Line text...", box);
    listIndexInput = new QLineEdit("0", box);
    listIndexInput->setMaximumWidth(40);

    QPushButton* insertBtn = new QPushButton("INSERT LINE", box);
    QPushButton* deleteBtn = new QPushButton("DELETE LINE", box);
    QPushButton* clearBtn = new QPushButton("Clear List", box);

    listVisualList = new QListWidget(box);
    listVisualList->setFont(QFont("Consolas", 10));
    listStatusLabel = new QLabel("Total Lines: 0", box);
    listStatusLabel->setStyleSheet("font-weight: bold; color: #b146c2;");

    grid->addWidget(new QLabel("Line Content:", box), 0, 0);
    grid->addWidget(listInput, 0, 1);
    grid->addWidget(new QLabel("Index:", box), 0, 2);
    grid->addWidget(listIndexInput, 0, 3);
    grid->addWidget(insertBtn, 0, 4);
    grid->addWidget(deleteBtn, 0, 5);
    grid->addWidget(clearBtn, 0, 6);
    grid->addWidget(listVisualList, 1, 0, 1, 7);
    grid->addWidget(listStatusLabel, 2, 0, 1, 7);

    layout->addWidget(box);

    connect(insertBtn, &QPushButton::clicked, this, &DsaGuideDialog::onListInsertClicked);
    connect(deleteBtn, &QPushButton::clicked, this, &DsaGuideDialog::onListDeleteClicked);
    connect(clearBtn, &QPushButton::clicked, this, &DsaGuideDialog::onListClearClicked);

    return widget;
}

QWidget* DsaGuideDialog::createHashTableTab() {
    QWidget* widget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(widget);

    QLabel* explLabel = new QLabel(
        "<b>Hash Table (Separate Chaining Collision Resolution)</b><br/>"
        "Used for: <b>Word Frequency Calculation, Fast Search, and Autocomplete</b>.<br/>"
        "<i>Why?</i> Provides average <b>O(1)</b> lookups to check if a word already exists and increment its frequency counter instantly without linear scanning.",
        widget
    );
    explLabel->setWordWrap(true);
    layout->addWidget(explLabel);

    QGroupBox* box = new QGroupBox("Interactive Hash Table Sandbox (Custom HashTable<std::string, int>)", widget);
    QGridLayout* grid = new QGridLayout(box);

    hashKeyInput = new QLineEdit("structure", box);
    hashValInput = new QLineEdit("1", box);
    hashValInput->setMaximumWidth(50);

    QPushButton* insertBtn = new QPushButton("INSERT / INCREMENT", box);
    QPushButton* searchBtn = new QPushButton("SEARCH", box);
    QPushButton* clearBtn = new QPushButton("Clear Hash Table", box);

    hashVisualDisplay = new QTextEdit(box);
    hashVisualDisplay->setReadOnly(true);
    hashVisualDisplay->setFont(QFont("Consolas", 10));
    hashStatusLabel = new QLabel("Unique Words: 0 | Buckets: 13", box);
    hashStatusLabel->setStyleSheet("font-weight: bold; color: #d83b01;");

    grid->addWidget(new QLabel("Word:", box), 0, 0);
    grid->addWidget(hashKeyInput, 0, 1);
    grid->addWidget(new QLabel("Frequency:", box), 0, 2);
    grid->addWidget(hashValInput, 0, 3);
    grid->addWidget(insertBtn, 0, 4);
    grid->addWidget(searchBtn, 0, 5);
    grid->addWidget(clearBtn, 0, 6);
    grid->addWidget(hashVisualDisplay, 1, 0, 1, 7);
    grid->addWidget(hashStatusLabel, 2, 0, 1, 7);

    layout->addWidget(box);

    connect(insertBtn, &QPushButton::clicked, this, &DsaGuideDialog::onHashInsertClicked);
    connect(searchBtn, &QPushButton::clicked, this, &DsaGuideDialog::onHashSearchClicked);
    connect(clearBtn, &QPushButton::clicked, this, &DsaGuideDialog::onHashClearClicked);

    return widget;
}

QWidget* DsaGuideDialog::createMergeSortAndRecursionTab() {
    QWidget* widget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(widget);

    QTextEdit* textEdit = new QTextEdit(widget);
    textEdit->setReadOnly(true);
    textEdit->setHtml(
        "<h3>🔀 Merge Sort (Divide & Conquer)</h3>"
        "<p><b>Where is it used?</b> Sorting document word frequencies descending for the Statistics Dialog and ranking Autocomplete suggestions.</p>"
        "<p><b>How it works:</b></p>"
        "<ol>"
        "<li><b>Divide:</b> Splits the frequency array into two halves at the midpoint.</li>"
        "<li><b>Conquer:</b> Recursively sorts each subarray.</li>"
        "<li><b>Combine (Merge):</b> Merges the sorted halves in linear O(n) auxiliary time.</li>"
        "</ol>"
        "<p><b>Why Merge Sort?</b> It is a <b>stable sort</b> with guaranteed <b>O(n log n)</b> performance in all cases (unlike QuickSort's worst-case O(n&sup2;)).</p>"
        "<hr/>"
        "<h3>🔁 Recursion (Linked List Traversal)</h3>"
        "<p><b>Where is it used?</b> Document statistics calculation (<code>countWordsInNodeRecursive</code> and <code>countCharsInNodeRecursive</code>).</p>"
        "<pre style='background: #f8f9fa; padding: 8px; border: 1px solid #ddd; font-family: Consolas;'>int countWordsInNodeRecursive(const Node&lt;std::string&gt;* node) {\n"
        "    if (node == nullptr) return 0; // Base case: end of linked list\n"
        "    return countInLine(node-&gt;data) + countWordsInNodeRecursive(node-&gt;next);\n"
        "}</pre>"
        "<p><b>Why Recursion?</b> Elegantly processes linked structures by reducing the problem to current node + remaining list until the base case.</p>"
    );

    layout->addWidget(textEdit);
    return widget;
}

// Sandbox Update Methods
void DsaGuideDialog::updateStackVisual() {
    stackVisualList->clear();
    // Reconstruct stack visual items
    Stack<std::string> temp = sandboxStack;
    std::vector<std::string> items;
    while (!temp.isEmpty()) {
        items.push_back(temp.pop());
    }

    for (size_t i = 0; i < items.size(); ++i) {
        QString label = (i == 0) ? QString("▲ TOP -> [ %1 ]").arg(QString::fromStdString(items[i]))
                                 : QString("         [ %1 ]").arg(QString::fromStdString(items[i]));
        stackVisualList->addItem(label);
    }
    if (items.empty()) {
        stackVisualList->addItem("(Stack is currently empty)");
    }
    stackStatusLabel->setText(QString("Stack Elements: %1 (LIFO Order)").arg(sandboxStack.size()));
}

void DsaGuideDialog::updateQueueVisual() {
    queueVisualList->clear();
    std::vector<std::string> items = sandboxQueue.toVector();

    for (size_t i = 0; i < items.size(); ++i) {
        QString marker;
        if (i == 0 && i == items.size() - 1) marker = "FRONT & REAR ->";
        else if (i == 0) marker = "FRONT (Next out) ->";
        else if (i == items.size() - 1) marker = "REAR  (Latest)   ->";
        else marker = "                   ";

        queueVisualList->addItem(QString("%1 [ %2 ]").arg(marker).arg(QString::fromStdString(items[i])));
    }
    if (items.empty()) {
        queueVisualList->addItem("(Queue is currently empty)");
    }
    queueStatusLabel->setText(QString("Queue Elements: %1 / 8 (FIFO Order)").arg(sandboxQueue.size()));
}

void DsaGuideDialog::updateListVisual() {
    listVisualList->clear();
    std::vector<std::string> lines = sandboxList.toVector();

    for (size_t i = 0; i < lines.size(); ++i) {
        QString arrow = (i == lines.size() - 1) ? "→ NULL" : "⇄";
        QString headMarker = (i == 0) ? "HEAD → " : "       ";
        listVisualList->addItem(QString("%1Node[%2]: \"%3\" %4").arg(headMarker).arg(i).arg(QString::fromStdString(lines[i])).arg(arrow));
    }
    if (lines.empty()) {
        listVisualList->addItem("HEAD → NULL (Document is empty)");
    }
    listStatusLabel->setText(QString("Total Lines: %1").arg(sandboxList.size()));
}

void DsaGuideDialog::updateHashVisual() {
    auto entries = sandboxHashTable.getAllEntries();
    QString text = "--- Hash Table Buckets (Separate Chaining) ---\n";
    for (size_t b = 0; b < sandboxHashTable.bucketCount(); ++b) {
        text += QString("Bucket [%1]: ").arg(b, 2);
        bool hasItem = false;
        for (const auto& e : entries) {
            // Simplified hash index calculation for display
            size_t h = 0;
            for (char c : e.first) h = (h * 31 + static_cast<unsigned char>(c)) % sandboxHashTable.bucketCount();
            if (h == b) {
                text += QString("\"%1\" (%2 freq) -> ").arg(QString::fromStdString(e.first)).arg(e.second);
                hasItem = true;
            }
        }
        text += "NULL\n";
    }
    hashVisualDisplay->setText(text);
    hashStatusLabel->setText(QString("Unique Words: %1 | Prime Buckets: %2").arg(sandboxHashTable.size()).arg(sandboxHashTable.bucketCount()));
}

// Slot Implementations
void DsaGuideDialog::onStackPushClicked() {
    QString val = stackInput->text().trimmed();
    if (!val.isEmpty()) {
        sandboxStack.push(val.toStdString());
        updateStackVisual();
        stackInput->setText("Action " + QString::number(sandboxStack.size() + 1));
    }
}

void DsaGuideDialog::onStackPopClicked() {
    if (!sandboxStack.isEmpty()) {
        sandboxStack.pop();
        updateStackVisual();
    }
}

void DsaGuideDialog::onStackClearClicked() {
    sandboxStack.clear();
    updateStackVisual();
}

void DsaGuideDialog::onQueueEnqueueClicked() {
    QString val = queueInput->text().trimmed();
    if (!val.isEmpty()) {
        if (sandboxQueue.size() >= 8) {
            sandboxQueue.dequeue();
        }
        sandboxQueue.enqueue(val.toStdString());
        updateQueueVisual();
        queueInput->setText("file_" + QString::number(sandboxQueue.size() + 1) + ".txt");
    }
}

void DsaGuideDialog::onQueueDequeueClicked() {
    if (!sandboxQueue.isEmpty()) {
        sandboxQueue.dequeue();
        updateQueueVisual();
    }
}

void DsaGuideDialog::onQueueClearClicked() {
    sandboxQueue.clear();
    updateQueueVisual();
}

void DsaGuideDialog::onListInsertClicked() {
    QString val = listInput->text().trimmed();
    int idx = listIndexInput->text().toInt();
    if (!val.isEmpty() && idx >= 0 && idx <= static_cast<int>(sandboxList.size())) {
        sandboxList.insert(idx, val.toStdString());
        updateListVisual();
    }
}

void DsaGuideDialog::onListDeleteClicked() {
    int idx = listIndexInput->text().toInt();
    if (idx >= 0 && idx < static_cast<int>(sandboxList.size())) {
        sandboxList.removeAt(idx);
        updateListVisual();
    }
}

void DsaGuideDialog::onListClearClicked() {
    sandboxList.clear();
    updateListVisual();
}

void DsaGuideDialog::onHashInsertClicked() {
    QString key = hashKeyInput->text().trimmed();
    int val = hashValInput->text().toInt();
    if (!key.isEmpty() && val > 0) {
        sandboxHashTable.incrementFrequency(key.toStdString(), val);
        updateHashVisual();
    }
}

void DsaGuideDialog::onHashSearchClicked() {
    QString key = hashKeyInput->text().trimmed();
    int val = 0;
    if (sandboxHashTable.search(key.toStdString(), val)) {
        hashStatusLabel->setText(QString("Found word '%1' with frequency: %2").arg(key).arg(val));
    } else {
        hashStatusLabel->setText(QString("Word '%1' not found in Hash Table.").arg(key));
    }
}

void DsaGuideDialog::onHashClearClicked() {
    sandboxHashTable.clear();
    updateHashVisual();
}
