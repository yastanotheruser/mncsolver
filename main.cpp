#include <QtDebug>
#include <QApplication>
#include "MclWindow.hpp"

QApplication *initApplication(int &argc, char **argv);
MclWindow *initMainWindow();

int main(int argc, char **argv)
{
    QApplication *app = initApplication(argc, argv);
    MclWindow *window = initMainWindow();
    window->show();
    return app->exec();
}

QApplication *initApplication(int &argc, char **argv)
{
    QApplication *app = new QApplication(argc, argv);
    QCoreApplication::setApplicationName("Misioneros y Caníbales");
    return app;
}

MclWindow *initMainWindow()
{
    MclWindow *window = new MclWindow();
    window->setWindowTitle(QCoreApplication::applicationName());
    window->setFixedSize(1280, 640);
    window->setFocusPolicy(Qt::ClickFocus);
    return window;
}
