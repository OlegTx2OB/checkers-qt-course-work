#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow* mainWindow = new MainWindow();
    mainWindow->show();
    QApplication::setWindowIcon(QIcon(":/icons/checkersIcon.png"));

    return a.exec();
}
