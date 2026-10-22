#include <iostream>
#include <cassert>
#include <string>
#include <vector>

#include "../DataStructures/Stack.h"
#include "../DataStructures/Queue.h"
#include "../DataStructures/LinkedList.h"
#include "../DataStructures/HashTable.h"
#include "../Algorithms/MergeSort.h"
#include "../Algorithms/BracketMatcher.h"
#include "../Algorithms/Search.h"
#include "../Editor/Document.h"
#include "../Editor/UndoManager.h"
#include "../Services/FileManager.h"
#include "../Services/StatisticsManager.h"
#include "../Services/Autocomplete.h"

void testStack() {
    std::cout << "[TEST] Running Stack tests..." << std::endl;
    Stack<int> s;
    assert(s.isEmpty());
    assert(s.size() == 0);

    s.push(10);
    s.push(20);
    s.push(30);
    assert(!s.isEmpty());
    assert(s.size() == 3);
    assert(s.top() == 30);

    assert(s.pop() == 30);
    assert(s.pop() == 20);
    assert(s.pop() == 10);
    assert(s.isEmpty());

    bool underflowCaught = false;
    try {
        s.pop();
    } catch (const std::underflow_error&) {
        underflowCaught = true;
    }
    assert(underflowCaught);

    for (int i = 0; i < 100; ++i) {
        s.push(i);
    }
    assert(s.size() == 100);
    for (int i = 99; i >= 0; --i) {
        assert(s.pop() == i);
    }
    std::cout << "  -> Stack tests passed successfully!" << std::endl;
}

void testQueue() {
    std::cout << "[TEST] Running Queue tests..." << std::endl;
    Queue<std::string> q(3);
    assert(q.isEmpty());

    q.enqueue("file1.txt");
    q.enqueue("file2.txt");
    q.enqueue("file3.txt");
    assert(q.size() == 3);
    assert(q.front() == "file1.txt");
    assert(q.contains("file2.txt"));
    assert(!q.contains("nonexistent.txt"));

    assert(q.dequeue() == "file1.txt");
    assert(q.front() == "file2.txt");

    q.enqueue("file4.txt");
    assert(q.size() == 3);

    bool removed = q.remove("file3.txt");
    assert(removed);
    assert(!q.contains("file3.txt"));
    assert(q.size() == 2);

    std::cout << "  -> Queue tests passed successfully!" << std::endl;
}

void testLinkedList() {
    std::cout << "[TEST] Running LinkedList tests..." << std::endl;
    LinkedList<std::string> doc;
    assert(doc.isEmpty());

    doc.push_back("Line 1");
    doc.push_back("Line 3");
    doc.insert(1, "Line 2");
    assert(doc.size() == 3);

    assert(doc.get(0) == "Line 1");
    assert(doc.get(1) == "Line 2");
    assert(doc.get(2) == "Line 3");

    doc.set(1, "Line 2 Updated");
    assert(doc.get(1) == "Line 2 Updated");

    doc.removeAt(1);
    assert(doc.size() == 2);
    assert(doc.get(0) == "Line 1");
    assert(doc.get(1) == "Line 3");

    doc.push_front("Line 0");
    assert(doc.get(0) == "Line 0");
    assert(doc.size() == 3);

    std::vector<std::string> vec = doc.toVector();
    assert(vec.size() == 3);
    assert(vec[0] == "Line 0");
    assert(vec[1] == "Line 1");
    assert(vec[2] == "Line 3");

    std::cout << "  -> LinkedList tests passed successfully!" << std::endl;
}

void testHashTable() {
    std::cout << "[TEST] Running HashTable tests..." << std::endl;
    HashTable<std::string, int> freqTable;
    assert(freqTable.isEmpty());

    freqTable.incrementFrequency("algorithm");
    freqTable.incrementFrequency("data");
    freqTable.incrementFrequency("structure");
    freqTable.incrementFrequency("data");
    freqTable.incrementFrequency("data");

    int val = 0;
    assert(freqTable.search("data", val) && val == 3);
    assert(freqTable.search("algorithm", val) && val == 1);
    assert(!freqTable.search("missing", val));

    freqTable.insert("custom", 42);
    assert(freqTable.search("custom", val) && val == 42);

    assert(freqTable.remove("structure"));
    assert(!freqTable.contains("structure"));

    for (int i = 0; i < 150; ++i) {
        freqTable.incrementFrequency("word_" + std::to_string(i), i + 1);
    }
    assert(freqTable.search("word_50", val) && val == 51);

    auto allEntries = freqTable.getAllEntries();
    assert(allEntries.size() == freqTable.size());

    std::cout << "  -> HashTable tests passed successfully!" << std::endl;
}

void testMergeSort() {
    std::cout << "[TEST] Running MergeSort tests..." << std::endl;
    std::vector<int> nums = {45, 12, 85, 32, 89, 39, 69, 44, 42, 1, 45};
    MergeSort::sort(nums);

    for (size_t i = 1; i < nums.size(); ++i) {
        assert(nums[i - 1] <= nums[i]);
    }

    std::vector<std::pair<std::string, int>> wordFreqs = {
        {"stack", 15},
        {"queue", 12},
        {"data", 27},
        {"algorithm", 21}
    };

    MergeSort::sort(wordFreqs, [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
        return a.second > b.second;
    });

    assert(wordFreqs[0].first == "data" && wordFreqs[0].second == 27);
    assert(wordFreqs[1].first == "algorithm" && wordFreqs[1].second == 21);
    assert(wordFreqs[2].first == "stack" && wordFreqs[2].second == 15);
    assert(wordFreqs[3].first == "queue" && wordFreqs[3].second == 12);

    std::cout << "  -> MergeSort tests passed successfully!" << std::endl;
}

void testBracketMatcher() {
    std::cout << "[TEST] Running BracketMatcher tests..." << std::endl;
    std::string validCode = "int main() { if (x[0] == 1) { return 0; } }";
    std::string errMsg;
    assert(BracketMatcher::validateBracketBalance(validCode, errMsg));

    std::string invalidCode = "int main() { if (x[0] == 1) { return 0; }";
    assert(!BracketMatcher::validateBracketBalance(invalidCode, errMsg));

    std::string mismatchedCode = "int main() { ( ] }";
    assert(!BracketMatcher::validateBracketBalance(mismatchedCode, errMsg));

    std::string sample = "function(a, [b, c])";
    auto match = BracketMatcher::findMatch(sample, 8);
    assert(match.isBracket);
    assert(match.matchPosition == 18);

    auto match2 = BracketMatcher::findMatch(sample, 12);
    assert(match2.isBracket);
    assert(match2.matchPosition == 17);

    std::cout << "  -> BracketMatcher tests passed successfully!" << std::endl;
}

void testSearch() {
    std::cout << "[TEST] Running Search tests..." << std::endl;
    std::vector<std::string> lines = {
        "Hello Data Structures",
        "Stack and Queue are fundamental structures",
        "Data structures are powerful"
    };

    auto matches = Search::searchInLines(lines, "structure", false);
    assert(matches.size() == 3);
    assert(matches[0].lineIndex == 0 && matches[0].startColumn == 11);

    std::cout << "  -> Search tests passed successfully!" << std::endl;
}

void testDocumentAndUndo() {
    std::cout << "[TEST] Running Document & UndoManager tests..." << std::endl;
    Document doc;
    doc.loadFromPlainText("Hello world\nThis is line 2\nLine 3 of document");
    assert(doc.lineCount() == 3);
    assert(doc.getRecursiveWordCount() == 10);

    UndoManager undoMgr;
    EditorAction act1(ActionType::INSERT_TEXT, 0, "Hello", "");
    undoMgr.recordAction(act1);
    assert(undoMgr.canUndo());
    assert(!undoMgr.canRedo());

    EditorAction popped = undoMgr.undo();
    assert(popped.text == "Hello");
    assert(undoMgr.canRedo());

    EditorAction redone = undoMgr.redo();
    assert(redone.text == "Hello");
    assert(undoMgr.canUndo());

    std::cout << "  -> Document & UndoManager tests passed!" << std::endl;
}

void testStatisticsAndAutocomplete() {
    std::cout << "[TEST] Running StatisticsManager & Autocomplete tests..." << std::endl;
    Document doc;
    doc.loadFromPlainText("data structure data algorithm stack data queue stack");
    auto stats = StatisticsManager::computeStatistics(doc, 5);

    assert(stats.wordCount == 8);
    assert(!stats.topWords.empty());
    assert(stats.topWords[0].first == "data" && stats.topWords[0].second == 3);
    assert(stats.topWords[1].first == "stack" && stats.topWords[1].second == 2);

    Autocomplete autoComp;
    auto suggestions = autoComp.getSuggestions("comp");
    assert(!suggestions.empty());
    assert(suggestions[0].substr(0, 4) == "comp");

    std::cout << "  -> StatisticsManager & Autocomplete tests passed!" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  Smart Text Editor - DSA Test Suite   " << std::endl;
    std::cout << "========================================" << std::endl;

    testStack();
    testQueue();
    testLinkedList();
    testHashTable();
    testMergeSort();
    testBracketMatcher();
    testSearch();
    testDocumentAndUndo();
    testStatisticsAndAutocomplete();

    std::cout << "\n>>> ALL DATA STRUCTURES & SERVICES TESTS PASSED! <<<\n" << std::endl;
    return 0;
}
