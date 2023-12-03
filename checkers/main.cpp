#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setWindowIcon(QIcon(":/icons/checkersIcon.png"));

    MainWindow* mainWindow = new MainWindow();
    mainWindow->show();

    return a.exec();
}
