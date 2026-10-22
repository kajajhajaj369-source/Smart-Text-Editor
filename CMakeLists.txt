cmake_minimum_required(VERSION 3.16)

project(SmartTextEditor
    VERSION 1.0.0
    DESCRIPTION "Smart Text Editor (Windows Notepad Style) - College Data Structures Course Project"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets)

# Header files
set(HEADERS
    DataStructures/Stack.h
    DataStructures/Queue.h
    DataStructures/LinkedList.h
    DataStructures/HashTable.h
    Algorithms/MergeSort.h
    Algorithms/BracketMatcher.h
    Algorithms/Search.h
    Editor/EditorAction.h
    Editor/Document.h
    Editor/UndoManager.h
    Services/DsaLogger.h
    Services/FileManager.h
    Services/StatisticsManager.h
    Services/Autocomplete.h
    UI/MainWindow.h
    UI/DsaGuideDialog.h
    UI/DsaActivityDialog.h
    UI/FindReplaceDialog.h
    UI/StatisticsDialog.h
)

# Source files
set(SOURCES
    main.cpp
    Editor/Document.cpp
    Editor/UndoManager.cpp
    Services/FileManager.cpp
    Services/StatisticsManager.cpp
    Services/Autocomplete.cpp
    UI/MainWindow.cpp
    UI/DsaGuideDialog.cpp
    UI/DsaActivityDialog.cpp
    UI/FindReplaceDialog.cpp
    UI/StatisticsDialog.cpp
)

# Main Application Executable (Windows Notepad Style)
add_executable(SmartTextEditor
    ${HEADERS}
    ${SOURCES}
)

target_link_libraries(SmartTextEditor PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
)

# DSA Standalone Unit Test Executable (Non-GUI, can run with plain g++/MSVC/Clang)
add_executable(test_dsa
    Tests/test_dsa.cpp
    Editor/Document.cpp
    Editor/UndoManager.cpp
    Services/FileManager.cpp
    Services/StatisticsManager.cpp
    Services/Autocomplete.cpp
)
