#include <QApplication>
#include "ui/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Scavenger Hunt");
    app.setOrganizationName("AUC-CSCE1102");

    MainWindow window;
    window.setWindowTitle("Scavenger Hunt");
    window.show();

    return app.exec();
}