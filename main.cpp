#include <QApplication>
#include "ui/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("scavengerhunt");
    QCoreApplication::setApplicationName("data");

    MainWindow window;
    window.setWindowTitle("Scavenger Hunt");
    window.show();

    return app.exec();
}