#ifndef DSA_GUIDE_DIALOG_H
#define DSA_GUIDE_DIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QListWidget>

#include "../DataStructures/Stack.h"
#include "../DataStructures/Queue.h"
#include "../DataStructures/LinkedList.h"
#include "../DataStructures/HashTable.h"

/**
 * @brief In-App Educational Data Structures & Algorithms Guide.
 * 
 * Provides viva explanations and live interactive test-benches for
 * Stack, Queue, Linked List, Hash Table, Merge Sort, and Recursion.
 */
class DsaGuideDialog : public QDialog {
    Q_OBJECT

public:
    explicit DsaGuideDialog(QWidget *parent = nullptr);
    ~DsaGuideDialog();

private slots:
    // Stack sandbox slots
    void onStackPushClicked();
    void onStackPopClicked();
    void onStackClearClicked();

    // Queue sandbox slots
    void onQueueEnqueueClicked();
    void onQueueDequeueClicked();
    void onQueueClearClicked();

    // Linked list sandbox slots
    void onListInsertClicked();
    void onListDeleteClicked();
    void onListClearClicked();

    // Hash table sandbox slots
    void onHashInsertClicked();
    void onHashSearchClicked();
    void onHashClearClicked();

private:
    QTabWidget* tabWidget;

    // Actual Data Structure Instances for Live Sandbox
    Stack<std::string> sandboxStack;
    Queue<std::string> sandboxQueue;
    LinkedList<std::string> sandboxList;
    HashTable<std::string, int> sandboxHashTable;

    // Stack UI controls
    QLineEdit* stackInput;
    QListWidget* stackVisualList;
    QLabel* stackStatusLabel;

    // Queue UI controls
    QLineEdit* queueInput;
    QListWidget* queueVisualList;
    QLabel* queueStatusLabel;

    // Linked list UI controls
    QLineEdit* listInput;
    QLineEdit* listIndexInput;
    QListWidget* listVisualList;
    QLabel* listStatusLabel;

    // Hash table UI controls
    QLineEdit* hashKeyInput;
    QLineEdit* hashValInput;
    QTextEdit* hashVisualDisplay;
    QLabel* hashStatusLabel;

    void setupUI();
    QWidget* createOverviewTab();
    QWidget* createStackTab();
    QWidget* createQueueTab();
    QWidget* createLinkedListTab();
    QWidget* createHashTableTab();
    QWidget* createMergeSortAndRecursionTab();

    void updateStackVisual();
    void updateQueueVisual();
    void updateListVisual();
    void updateHashVisual();
};

#endif // DSA_GUIDE_DIALOG_H
