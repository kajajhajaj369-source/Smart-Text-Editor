# Smart Text Editor — Windows Notepad Style (DSA Course Project)

A complete, functional **Windows Notepad-style Smart Text Editor** developed in **C++** and **Qt 6** to demonstrate practical applications of college Data Structures and Algorithms (**Arrays, Linked Lists, Stacks, Queues, Hash Tables, Merge Sort, and Recursion**).

---

## 1. Design Philosophy

The application features an intentional **dual-layer design**:

### Layer 1: Clean Windows Notepad Experience (Normal Mode)
- Looks, feels, and behaves like standard Windows Notepad.
- Traditional menu structure: `File`, `Edit`, `Format` (Word Wrap, Font), `View`, `Help`.
- Discrete, uncluttered status bar: `Ln 12, Col 8 | 245 words | 1,420 characters`.
- No intrusive DSA clutter during everyday typing.

### Layer 2: In-App Data Structures Laboratory (Viva Mode)
- **In-App DSA Guide (`View → Data Structures Guide` or `F1`)**: Dedicated educational panel with cards for each concept and live interactive visualizers (`[Try Stack]`, `[Try Queue]`, `[Try Linked List]`, `[Try Hash Table]`).
- **Live Action Mapping HUD (`View → Live DSA Mapping`)**: Optional educational overlay displaying the active Data Structure, Operation (`POP`, `ENQUEUE`, `SEARCH`), and Big-O Complexity upon every keystroke or command.
- **DSA Activity Log (`View → DSA Activity Log` or `F8`)**: Timestamped audit trail of all underlying operations with a `Clear Activity` button.
- **Statistics (`View → Statistics` or `F4`)**: Real-time metrics calculated recursively and word frequencies ranked via custom Merge Sort.

---

## 2. Data Structure & Algorithm Mapping

```
                                +---------------------------------------+
                                |    MainWindow (Windows Notepad UI)    |
                                +-------------------+-------------------+
                                                    |
       +--------------------------------------------+--------------------------------------------+
       |                                            |                                            |
+------v------+                            +--------v--------+                          +--------v--------+
|   Editor    |                            |    DSA Guide    |                          | Activity / HUD  |
|  Document   |                            | Visualizer Cards|                          | Live Log Stream |
+------+------+                            +--------+--------+                          +--------+--------+
       |                                            |                                            |
       +--------------------+-----------------------+--------------------+-----------------------+
                            |                                            |
               +------------v------------+                  +------------v------------+
               |  Custom Data Structures |                  |    Custom Algorithms    |
               +-------------------------+                  +-------------------------+
               | • Stack<T>              |                  | • MergeSort<T>          |
               | • Queue<T>              |                  | • BracketMatcher        |
               | • LinkedList<T>         |                  | • RecursiveTraversal    |
               | • HashTable<K, V>       |                  | • Search (Pattern Match)|
               +-------------------------+                  +-------------------------+
```

| Application Feature | Underlying Data Structure / Algorithm | Operation | Time Complexity | Viva Defense Rationale |
| :--- | :--- | :--- | :--- | :--- |
| **Undo / Redo** | `Stack<EditorAction>` | `PUSH`, `POP` | $O(1)$ amortized | LIFO behavior reverses the most recent editing operation first. |
| **Document Lines** | `LinkedList<LineNode>` | `INSERT`, `DELETE`, `TRAVERSE` | $O(1)$ head/tail, $O(n)$ index | Dynamic non-contiguous node allocation avoids shifting large memory blocks. |
| **Recent Files** | `Queue<std::string>` | `ENQUEUE`, `DEQUEUE` | $O(1)$ circular buffer | FIFO behavior automatically purges the oldest file when capacity (10) is reached. |
| **Word Frequency / Search** | `HashTable<std::string, int>` | `INSERT`, `SEARCH`, `UPDATE` | $O(1)$ average | Separate chaining handles collisions cleanly; polynomial rolling hash indexes words instantly. |
| **Word Stats Ranking** | `MergeSort<Pair<string, int>>`| Divide $\to$ Sort $\to$ Merge | $O(n \log n)$ guaranteed | Stable divide-and-conquer sorting guarantees performance even on worst-case inputs. |
| **Document Stats Traversal**| **Recursion** | `countWords(node)` $\to$ `next` | $O(\text{lines})$ call stack | Solves line-by-line metrics recursively until `node == nullptr` (base case). |
| **Bracket Matching** | `Stack<char>` | `PUSH` open, `POP` match | $O(n)$ text scan | Verifies balanced nested delimiters `()`, `{}`, `[]` via LIFO matching. |

---

## 3. Project Directory Structure

```text
SmartTextEditor/
│
├── CMakeLists.txt              # Standard Qt 6 / Qt 5 CMake build configuration
├── main.cpp                    # Application bootstrap
├── README.md                   # Comprehensive Viva Guide & Documentation
├── Open_App.bat                # 1-Click launcher for interactive Notepad web application
├── Run_DSA_Verification.bat    # 1-Click launcher for standalone C++ unit tests
│
├── DataStructures/
│   ├── Stack.h                 # Dynamic array Stack template (PUSH, POP, TOP, RESIZE)
│   ├── Queue.h                 # Circular buffer Queue template (ENQUEUE, DEQUEUE, CONTAINS)
│   ├── LinkedList.h            # Doubly-linked list template (INSERT, REMOVE, GET, SET)
│   └── HashTable.h             # Separate chaining Hash Table (INSERT, SEARCH, INCREMENT, REMOVE)
│
├── Algorithms/
│   ├── MergeSort.h             # Recursive stable Merge Sort algorithm
│   ├── BracketMatcher.h        # Stack-based delimiter matching & balance check
│   └── Search.h                # Recursive pattern matching & multi-line occurrence search
│
├── Editor/
│   ├── EditorAction.h          # Action record struct (INSERT, DELETE, REPLACE)
│   ├── Document.h              # Document interface backed by custom LinkedList
│   ├── Document.cpp
│   ├── UndoManager.h           # Dual-stack Undo/Redo coordinator
│   └── UndoManager.cpp
│
├── Services/
│   ├── DsaLogger.h             # Real-time DSA audit logger for HUD & Activity Log
│   ├── FileManager.h           # File I/O + Recent Files Queue integration
│   ├── FileManager.cpp
│   ├── StatisticsManager.h     # Recursive stats & Hash Table frequencies
│   ├── StatisticsManager.cpp
│   ├── Autocomplete.h          # Prefix engine via Hash Table & MergeSort
│   └── Autocomplete.cpp
│
├── UI/
│   ├── MainWindow.h            # Windows Notepad-style main window
│   ├── MainWindow.cpp
│   ├── DsaGuideDialog.h        # Interactive in-app DSA Guide & Live visualizers
│   ├── DsaGuideDialog.cpp
│   ├── DsaActivityDialog.h     # Real-time DSA Activity Log dialog
│   ├── DsaActivityDialog.cpp
│   ├── FindReplaceDialog.h     # Notepad-style Find & Replace dialog
│   ├── FindReplaceDialog.cpp
│   ├── StatisticsDialog.h      # Document Statistics dialog
│   └── StatisticsDialog.cpp
│
└── Tests/
    └── test_dsa.cpp            # Standalone unit test suite
```

---

## 4. How to Open and Run

### 1-Click Instant Open:
- Double-click **`Open_App.bat`** to launch the complete interactive Notepad application with built-in interactive DSA guide directly in your browser.
- Double-click **`Run_DSA_Verification.bat`** to run the standalone C++ data structures test runner with a single click.

### In Qt Creator (Recommended for College Viva Demo):
1. Open **Qt Creator**.
2. Go to **File → Open File or Project...** and select `CMakeLists.txt`.
3. Choose your configured Qt 6 kit (MinGW 64-bit or MSVC).
4. Click **Configure Project** and press **Ctrl+R** to build and run.

### From Command Line:
```bash
# Build standalone test suite
g++ -std=c++14 Tests/test_dsa.cpp Editor/Document.cpp Editor/UndoManager.cpp Services/FileManager.cpp Services/StatisticsManager.cpp Services/Autocomplete.cpp -o test_dsa.exe
./test_dsa.exe
```

---

## 5. Viva / Oral Exam Quick Reference

- **"Where did you use Stack?"**  
  $\to$ **Undo & Redo** (`undoStack`, `redoStack`) and **Bracket Matching** (`()` `{}` `[]`).  
  *Rationale:* LIFO behavior reverses the most recent edit action first.
- **"Where did you use Queue?"**  
  $\to$ **Recent Files List** (`Queue<std::string>`).  
  *Rationale:* FIFO circular buffer evicts oldest entry when capacity is exceeded.
- **"Where did you use Linked List?"**  
  $\to$ **Document Lines** (`LinkedList<std::string>`).  
  *Rationale:* Allows dynamic line insertion/deletion without shifting contiguous memory buffers.
- **"Where did you use Hash Table?"**  
  $\to$ **Word Frequency Index & Autocomplete** (`HashTable<std::string, int>`).  
  *Rationale:* Separate chaining with polynomial rolling hash achieves $O(1)$ average lookup and update time.
- **"Where did you use Sorting?"**  
  $\to$ **Word Statistics Ranking** (`MergeSort`).  
  *Rationale:* Guaranteed $O(n \log n)$ stable divide-and-conquer sorting.
- **"Where did you use Recursion?"**  
  $\to$ **Document Statistics Traversal** (`countWordsInNodeRecursive`).  
  *Rationale:* Traverses linked line nodes recursively until reaching the `nullptr` base case.
