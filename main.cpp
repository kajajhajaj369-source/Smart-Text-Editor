#include <QApplication>
#include "UI/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Smart Text Editor");
    app.setApplicationDisplayName("Notepad — Smart Text Editor (DSA Lab Edition)");
    app.setOrganizationName("DSA Department");

    MainWindow window;
    window.show();

    return app.exec();
}
